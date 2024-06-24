#include "../../globals.h"
#include "../../DB_Handler/db_handler.h"
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>

#include <iostream>

void tables_page(int &logo_add, int &logo_pos, int *screenW, ImGuiStyle *style);

namespace tables
{
    void tables_page(int &logo_add, int &logo_pos, int *screenW, ImGuiStyle *style)
    {
        style->Colors[ImGuiCol_Text] = ImColor(171, 92, 255);
        ImGui::SetCursorPos(ImVec2((*screenW / 2) - 80, logo_pos));
        ImGui::PushFont(globals.logo_font);
        ImGui::Text("Tables");
        ImGui::PopFont();
        style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
        ImGui::SetCursorPos(ImVec2((*screenW / 2) + 2 - 80, logo_pos + 2));
        ImGui::PushFont(globals.logo_font);
        ImGui::Text("Tables");
        ImGui::PopFont();
    }
}