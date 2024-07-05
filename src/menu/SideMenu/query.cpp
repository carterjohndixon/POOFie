#include "../../globals.h"
#include "../../DB_Handler/db_handler.h"
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>

#include <iostream>

void query_page(int &logo_add, int &logo_pos, float *screenW, ImGuiStyle *style);

namespace query
{
    void query_page(int &logo_add, int &logo_pos, float *screenW, ImGuiStyle *style)
    {
        style->Colors[ImGuiCol_Text] = ImColor(171, 92, 255);
        ImGui::SetCursorPos(ImVec2((*screenW / 2) - 80, logo_pos));
        ImGui::PushFont(globals.logo_font);
        ImGui::Text("Query");
        ImGui::PopFont();
        style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
        ImGui::SetCursorPos(ImVec2((*screenW / 2) + 2 - 80, logo_pos + 2));
        ImGui::PushFont(globals.logo_font);
        ImGui::Text("Query");
        ImGui::PopFont();
    }
}