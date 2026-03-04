#include "../../log/Logger.h"
#include "ShaderRelfection.h"
#include <d3dcompiler.h>
#include <windows.h>
#include <wrl/client.h>

std::optional<D3D11_SHADER_INPUT_BIND_DESC> ShaderRelfection::get_desc(
    void* const data,
    const uint32_t size,
    const std::function<bool(const D3D11_SHADER_INPUT_BIND_DESC&)>& function
) {
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflector;
    {
        const HRESULT hr = D3DReflect(
            data,
            size,
            IID_PPV_ARGS(&reflector)
        );

        if(FAILED(hr)) {
            Logger::error(hresult_message(hr));
            return std::optional<D3D11_SHADER_INPUT_BIND_DESC>();
        }
    }

    D3D11_SHADER_DESC shader_desc{};
    {
        const HRESULT hr = reflector->GetDesc(&shader_desc);
        if(FAILED(hr)) {
            Logger::error(hresult_message(hr));
            return std::optional<D3D11_SHADER_INPUT_BIND_DESC>();
        }
    }

    for(UINT i = 0; i < shader_desc.BoundResources; ++i) {
        D3D11_SHADER_INPUT_BIND_DESC bind_desc{};
        const HRESULT hr = reflector->GetResourceBindingDesc(i, &bind_desc);
        if(FAILED(hr)) {
            Logger::warning(hresult_message(hr));
            continue;
        }

        if(function(bind_desc)) {
            return bind_desc;
        }
    }

    return std::optional<D3D11_SHADER_INPUT_BIND_DESC>();
}

void ShaderRelfection::get_all_desc(
    void* const data,
    const uint32_t size,
    const std::function<void(const D3D11_SHADER_INPUT_BIND_DESC&)>& function
) {
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflector;
    {
        const HRESULT hr = D3DReflect(
            data,
            size,
            IID_PPV_ARGS(&reflector)
        );

        if(FAILED(hr)) {
            Logger::error(hresult_message(hr));
            return;
        }
    }

    D3D11_SHADER_DESC shader_desc{};
    {
        const HRESULT hr = reflector->GetDesc(&shader_desc);
        if(FAILED(hr)) {
            Logger::error(hresult_message(hr));
            return;
        }
    }

    for(UINT i = 0; i < shader_desc.BoundResources; ++i) {
        D3D11_SHADER_INPUT_BIND_DESC bind_desc{};
        const HRESULT hr = reflector->GetResourceBindingDesc(i, &bind_desc);
        if(FAILED(hr)) {
            Logger::warning(hresult_message(hr));
            continue;
        }

        function(bind_desc);
    }
}

std::optional<uint32_t> ShaderRelfection::get_constant_buffer_slot(
    void* const data,
    const uint32_t size,
    const std::string& name
) {
    const auto opt_desc = ShaderRelfection::get_desc(
        data,
        size,
        [&name](const D3D11_SHADER_INPUT_BIND_DESC& desc) -> bool {
            return desc.Type == D3D_SIT_CBUFFER && name == desc.Name;
        }
    );
    if(!opt_desc.has_value()) {
        return std::optional<uint32_t>();
    }
    const auto& desc = opt_desc.value();

    return desc.BindPoint;
}

std::optional<uint32_t> ShaderRelfection::get_sampler_slot(
    void* const data,
    const uint32_t size,
    const std::string& name
) {
    const auto opt_desc = ShaderRelfection::get_desc(
        data,
        size,
        [&name](const D3D11_SHADER_INPUT_BIND_DESC& desc) -> bool {
            return desc.Type == D3D_SIT_SAMPLER && name == desc.Name;
        }
    );
    if(!opt_desc.has_value()) {
        return std::optional<uint32_t>();
    }
    const auto& desc = opt_desc.value();

    return desc.BindPoint;
}

std::optional<uint32_t> ShaderRelfection::get_texture_slot(
    void* const data,
    const uint32_t size,
    const std::string& name
) {
    const auto opt_desc = ShaderRelfection::get_desc(
        data,
        size,
        [&name](const D3D11_SHADER_INPUT_BIND_DESC& desc) -> bool {
            return desc.Type == D3D_SIT_TEXTURE && name == desc.Name;
        }
    );
    if(!opt_desc.has_value()) {
        return std::optional<uint32_t>();
    }
    const auto& desc = opt_desc.value();

    return desc.BindPoint;
}

std::optional<uint32_t> ShaderRelfection::get_texture_slot(
    const std::vector<uint8_t>& data,
    const std::string& name
) {
    return ShaderRelfection::get_texture_slot(
        (void*)data.data(),
        static_cast<uint32_t>(data.size()),
        name
    );
}

std::optional<uint32_t> ShaderRelfection::get_unordered_access_view_slot(void* const data, const uint32_t size, const std::string& name) {
    const auto opt_desc = ShaderRelfection::get_desc(
        data,
        size,
        [&name](const D3D11_SHADER_INPUT_BIND_DESC& desc) -> bool {
            if(name != desc.Name) {
                return false;
            }

            switch(desc.Type) {
            case D3D_SIT_UAV_APPEND_STRUCTURED:
            case D3D_SIT_UAV_CONSUME_STRUCTURED:
            case D3D_SIT_UAV_FEEDBACKTEXTURE:
            case D3D_SIT_UAV_RWBYTEADDRESS:
            case D3D_SIT_UAV_RWSTRUCTURED:
            case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
            case D3D_SIT_UAV_RWTYPED:
                return true;
            default:
                return false;
            }
        }
    );
    if(!opt_desc.has_value()) {
        return std::optional<uint32_t>();
    }
    const auto& desc = opt_desc.value();

    return desc.BindPoint;
}

std::optional<uint32_t> ShaderRelfection::get_unordered_access_view_slot(const std::vector<uint8_t>& data, const std::string& name) {
    return ShaderRelfection::get_unordered_access_view_slot(
        (void*)data.data(),
        static_cast<uint32_t>(data.size()),
        name
    );
}

std::optional<uint32_t> ShaderRelfection::get_constant_buffer_slot(const std::vector<uint8_t>& data, const std::string& name) {
    return ShaderRelfection::get_constant_buffer_slot(
        (void*)data.data(),
        static_cast<uint32_t>(data.size()),
        name
    );
}

std::optional<uint32_t> ShaderRelfection::get_sampler_slot(
    const std::vector<uint8_t>& data,
    const std::string& name
) {
    return ShaderRelfection::get_sampler_slot(
        (void*)data.data(),
        static_cast<uint32_t>(data.size()),
        name
    );
}

std::optional<InputElements> ShaderRelfection::get_input_elements(
    const std::vector<uint8_t>& data,
    const uint32_t& input_slot
) {
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflector;
    {
        const HRESULT hr = D3DReflect(
            (void*)data.data(),
            data.size(),
            IID_PPV_ARGS(&reflector)
        );

        if(FAILED(hr)) {
            Logger::error(hresult_message(hr));
            return std::nullopt;
        }
    }

    D3D11_SHADER_DESC shader_desc;
    {
        const HRESULT hr = reflector->GetDesc(&shader_desc);
        if(FAILED(hr)) {
            Logger::error(hresult_message(hr));
            return std::nullopt;
        }
    }

    InputElements input_elements;
    for(decltype(shader_desc.InputParameters) i = 0; i < shader_desc.InputParameters; ++i) {
        D3D11_SIGNATURE_PARAMETER_DESC param_desc;
        const HRESULT hr = reflector->GetInputParameterDesc(i, &param_desc);
        if(FAILED(hr)) {
            Logger::error(hresult_message(hr));
            return std::nullopt;
        }

        // マスクからフォーマットを判定 (R, G, B, A のどれが使われているか)
        DXGI_FORMAT fromat = DXGI_FORMAT_UNKNOWN;
        if(param_desc.Mask == 1) {
            if(param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
                fromat = DXGI_FORMAT_R32_UINT;
            } else if(param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
                fromat = DXGI_FORMAT_R32_SINT;
            } else if(param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
                fromat = DXGI_FORMAT_R32_FLOAT;
            }
        } else if(param_desc.Mask <= 3) {
            if(param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
                fromat = DXGI_FORMAT_R32G32_UINT;
            } else if(param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
                fromat = DXGI_FORMAT_R32G32_SINT;
            } else if(param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
                fromat = DXGI_FORMAT_R32G32_FLOAT;
            }
        } else if(param_desc.Mask <= 7) {
            if(param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
                fromat = DXGI_FORMAT_R32G32B32_UINT;
            } else if(param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
                fromat = DXGI_FORMAT_R32G32B32_SINT;
            } else if(param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
                fromat = DXGI_FORMAT_R32G32B32_FLOAT;
            }
        } else if(param_desc.Mask <= 15) {
            if(param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
                fromat = DXGI_FORMAT_R32G32B32A32_UINT;
            } else if(param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
                fromat = DXGI_FORMAT_R32G32B32A32_SINT;
            } else if(param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
                fromat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            }
        }

        if(fromat == DXGI_FORMAT_UNKNOWN) {
            return std::nullopt;
        }

        const D3D11_INPUT_ELEMENT_DESC element_desc = {
            .SemanticName = param_desc.SemanticName,
            .SemanticIndex = param_desc.SemanticIndex,
            .Format = fromat,
            .InputSlot = input_slot,
            .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT, // 自動オフセット
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        };

        input_elements.add(element_desc);
    }

    return input_elements;
}