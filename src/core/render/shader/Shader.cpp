#include "../../utility/BinaryReader.h"
#include "../../utility/Hash.h"
#include "Shader.h"
#include "ShaderRelfection.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <wrl/client.h>

constexpr uint32_t SHADER_MAGIC_NUMBER = 0x43425844;

bool Shader::is_compiled_shader_file(const std::filesystem::path& path) {
    auto opt_br = BinaryReader::make_reader(path);
    if(!opt_br.has_value()) {
        return false;
    }
    auto& br = opt_br.value();

    uint32_t id = 0;
    if(!br.read_to(&id)) {
        return false;
    }

    return id == SHADER_MAGIC_NUMBER;
}

Shader::Shader(std::unique_ptr<IShaderBlueprint> blueprint) {
    this->blueprint = std::move(blueprint);
}

bool Shader::compile_vertex_shader(ID3D11Device* const device, ID3D11VertexShader** const vertex_shader) {
    Microsoft::WRL::ComPtr<ID3DBlob> vs_blob;
    if(!this->compile_shader(vs_blob.GetAddressOf())) {
        return false;
    }

    const HRESULT hr = device->CreateVertexShader(
        vs_blob->GetBufferPointer(),
        vs_blob->GetBufferSize(),
        nullptr,
        vertex_shader
    );
    if(FAILED(hr)) {
        return false;
    }

    const uint8_t* data = static_cast<const uint8_t*>(vs_blob->GetBufferPointer());
    const size_t size = vs_blob->GetBufferSize();
    this->shader_raw_data = std::vector<uint8_t>(data, data + size);

    return true;
}

bool Shader::compile_pixel_shader(ID3D11Device* const device, ID3D11PixelShader** const pixel_shader) {
    Microsoft::WRL::ComPtr<ID3DBlob> ps_blob;
    if(!this->compile_shader(ps_blob.GetAddressOf())) {
        return false;
    }

    const HRESULT hr = device->CreatePixelShader(
        ps_blob->GetBufferPointer(),
        ps_blob->GetBufferSize(),
        nullptr,
        pixel_shader
    );
    if(FAILED(hr)) {
        return false;
    }

    const uint8_t* data = static_cast<const uint8_t*>(ps_blob->GetBufferPointer());
    const size_t size = ps_blob->GetBufferSize();
    this->shader_raw_data = std::vector<uint8_t>(data, data + size);

    return true;
}

bool Shader::make_vertex_shader(ID3D11Device* const device, ID3D11VertexShader** const vertex_shader) {
    if(this->blueprint->shader_type() != ShaderType::Vertex) {
        return false;
    }

    if(!Shader::is_compiled_shader_file(this->blueprint->file_path())) {
        return this->compile_vertex_shader(device, vertex_shader);
    }

    if(!this->read_shader()) {
        return false;
    }

    const HRESULT hr = device->CreateVertexShader(
        this->shader_raw_data.data(),
        this->shader_raw_data.size(),
        nullptr,
        vertex_shader
    );
    if(FAILED(hr)) {
        return false;
    }

    return true;
}

bool Shader::make_pixel_shader(ID3D11Device* const device, ID3D11PixelShader** const pixel_shader) {
    if(this->blueprint->shader_type() != ShaderType::Pixel) {
        return false;
    }

    if(!Shader::is_compiled_shader_file(this->blueprint->file_path())) {
        return this->compile_pixel_shader(device, pixel_shader);
    }

    if(!this->read_shader()) {
        return false;
    }

    const HRESULT hr = device->CreatePixelShader(
        this->shader_raw_data.data(),
        this->shader_raw_data.size(),
        nullptr,
        pixel_shader
    );
    if(FAILED(hr)) {
        return false;
    }

    return true;
}

bool Shader::read_shader(void) {
    if(!this->shader_raw_data.empty()) {
        this->shader_raw_data.clear();
    }

    auto opt_br = BinaryReader::make_reader(this->blueprint->file_path());
    if(!opt_br.has_value()) {
        return false;
    }
    auto& br = opt_br.value();

    this->shader_raw_data = br.read_all();
    if(this->shader_raw_data.empty()) {
        return false;
    }

    return true;
}

bool Shader::compile_shader(ID3DBlob** const blob) {
    Microsoft::WRL::ComPtr<ID3DBlob> error_blob;

    const HRESULT hr = D3DCompileFromFile(
        this->blueprint->file_path().c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        this->blueprint->main_function_name().c_str(),
        this->blueprint->version().c_str(),
        0,
        0,
        blob,
        error_blob.GetAddressOf()
    );

    if(FAILED(hr)) {
        if(error_blob.Get()) {
            OutputDebugStringA((char*)error_blob->GetBufferPointer());
        }
        return false;
    }

    return true;
}

bool Shader::make_input_layout(
    ID3D11Device* const device,
    ID3D11InputLayout** const input_layout
) {
    if(this->shader_raw_data.empty()) {
        return false;
    }

    const auto opt_input_elements = ShaderRelfection::get_input_elements(
        this->shader_raw_data,
        0 // 通常は0
    );
    if(!opt_input_elements.has_value()) {
        return false;
    }
    const auto& input_elements = opt_input_elements.value();

    const HRESULT hr = device->CreateInputLayout(
        input_elements.data(),
        static_cast<uint32_t>(input_elements.size()),
        this->shader_raw_data.data(),
        this->shader_raw_data.size(),
        input_layout
    );
    if(FAILED(hr)) {
        return false;
    }

    return true;
}

std::optional<uint32_t> Shader::get_constant_buffer_slot(const std::string& name) const {
    return ShaderRelfection::get_constant_buffer_slot(
        this->shader_raw_data,
        name
    );
}

bool Shader::read_constant_buffer_slot(uint32_t& slot, const std::string& name) const {
    const auto& opt = this->get_constant_buffer_slot(name);
    if(!opt.has_value()) {
        return false;
    }
    slot = opt.value();

    return true;
}

std::optional<uint32_t> Shader::get_sampler_state_slot(const std::string& name) const {
    return ShaderRelfection::get_sampler_slot(
        this->shader_raw_data,
        name
    );
}

std::optional<uint32_t> Shader::get_texture_slot(const std::string& name) const {
    return ShaderRelfection::get_texture_slot(
        this->shader_raw_data,
        name
    );
}

bool Shader::read_sampler_state_slot(uint32_t& slot, const std::string& name) const {
    const auto& opt = this->get_sampler_state_slot(name);
    if(!opt.has_value()) {
        return false;
    }
    slot = opt.value();

    return true;
}

const IShaderBlueprint* Shader::get_blueprint(void) const {
    return this->blueprint.get();
}

std::unordered_map<BindingSlotKey, uint32_t> Shader::get_all_slots(void) const {
    std::unordered_map<BindingSlotKey, uint32_t> map;

    std::cout << "===========================" << std::endl;

    ShaderRelfection::get_all_desc(
        (void*)this->shader_raw_data.data(),
        static_cast<uint32_t>(this->shader_raw_data.size()),
        [&](const D3D11_SHADER_INPUT_BIND_DESC& desc) {
            BindingSlotKind kind{};

            switch(desc.Type) {
            case D3D10_SHADER_INPUT_TYPE::D3D10_SIT_CBUFFER:
                kind = BindingSlotKind::ConstantBuffer;
                break;
            case D3D10_SHADER_INPUT_TYPE::D3D10_SIT_SAMPLER:
                kind = BindingSlotKind::SamplerState;
                break;
            case D3D10_SHADER_INPUT_TYPE::D3D10_SIT_TEXTURE:
                kind = BindingSlotKind::Texture;
                break;
            default:
                return;
            }

            const BindingSlotKey key{
                hash_u32(desc.Name),
                this->blueprint->shader_type(),
                kind
            };

            std::cout << "--------------------" << std::endl;
            std::cout << "Name: " << desc.Name << std::endl;
            std::cout << "NameHash: " << hash_u32(desc.Name) << std::endl;
            std::cout << "Point: " << desc.BindPoint << std::endl;
            std::cout << "Type: " << desc.Type << std::endl;

            map.emplace(key, desc.BindPoint);
        }
    );

    return map;
}