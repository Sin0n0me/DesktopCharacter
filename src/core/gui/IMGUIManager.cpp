#include "editor/ScriptEditorWindow.h"
#include "IMGUIManager.h"
#include "menu/MenuWindow.h"
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <imgui.h>

IMGUIManager::IMGUIManager(void) {
    this->menu = std::make_unique<MenuWindow>();
    this->editor = std::make_unique<ScriptEditorWindow>();
}

IMGUIManager::~IMGUIManager(void) noexcept {
}

bool IMGUIManager::init(void) {
    if(!this->editor->init()) {
        return false;
    }

    return true;
}

void IMGUIManager::update(void) {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
        //this->menu->update();
        this->editor->update();
    }

    ImGui::Render();
}

void IMGUIManager::render(void) const {
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}