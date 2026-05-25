#include "MenuWindow.h"
#include <imgui.h>

void MenuWindow::on_clicked_option(void) {
}

void MenuWindow::on_clicked_exit(void) {
}

void MenuWindow::update(void) {
    ImGui::Begin("Menu Window");

    if(ImGui::Button("Option", ImVec2(200, 40))) {
        this->on_clicked_option();
    }

    if(ImGui::Button("Exit", ImVec2(200, 40))) {
        this->on_clicked_exit();
    }

    ImGui::End();
}