#include "../../../Application.h"
#include "../../script/Script.h"
#include "ScriptEditorWindow.h"
#include <imgui.h>

constexpr int TEXT_BUFFER_SIZE = 0x4000;
constexpr float BUTTON_HEIGHT = 30.0f;
constexpr float FONT_SIZE_LARGE = 24.0f;

ScriptEditorWindow::ScriptEditorWindow(void) {
    this->text_buffer.resize(TEXT_BUFFER_SIZE);

    std::u8string code =
        u8"fn main(input: matrix) -> matrix {\n"
        u8"        let translate = matrix(   \n"
        u8"            1.0, 0.0, 0.0, 0.0,   \n"
        u8"            0.0, 1.0, 0.0, 0.0,   \n"
        u8"            0.0, 0.0, 1.0, 0.0,   \n"
        u8"            0.0, 0.0, 0.0, 1.0,   \n"
        u8"        );                        \n"
        u8"    return input * translate;     \n"
        u8"}                                 \n";

    std::copy(
        reinterpret_cast<const char*>(code.data()),
        reinterpret_cast<const char*>(code.data()) + code.size(),
        this->text_buffer.begin()
    );
}

bool ScriptEditorWindow::init(void) {
    ImGuiIO& io = ImGui::GetIO();
    this->font = io.Fonts->AddFontFromFileTTF(
        "ipaexm.ttf",
        FONT_SIZE_LARGE
    );

    if(!this->font) {
        return false;
    }

    return true;
}

void ScriptEditorWindow::on_clicked_compile(void) {
    Script::script_compile(std::u8string(
        reinterpret_cast<const char8_t*>(this->text_buffer.data()),
        400
    ));
}

void ScriptEditorWindow::update(void) {
    ImGui::SetNextWindowSize(
        ImVec2(WIDTH / 3, HEIGHT / 3),
        ImGuiCond_Once
    );

    if(!ImGui::Begin("Editor Window")) {
        ImGui::End();
        return;
    }

    // 利用可能領域取得
    const ImVec2 content_region = ImGui::GetContentRegionAvail();

    // ボタン分の高さを確保
    float input_height = content_region.y - BUTTON_HEIGHT - ImGui::GetStyle().ItemSpacing.y;
    if(input_height < 0.0f) {
        input_height = 0.0f;
    }

    ImGui::PushFont(this->font);

    const ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_AllowTabInput;
    ImGui::InputTextMultiline(
        "##Editor",
        this->text_buffer.data(),
        TEXT_BUFFER_SIZE,
        ImVec2(content_region.x, input_height),
        inputFlags
    );

    ImGui::PopFont();

    if(ImGui::Button("Compile", ImVec2(-FLT_MIN, BUTTON_HEIGHT))) {
        this->on_clicked_compile();
    }

    ImGui::End();
}