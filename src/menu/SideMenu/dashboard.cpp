#include "../../globals.h"
#include "../../DB_Handler/db_handler.h"
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>

#include <iostream>

void dashboard_page(int &logo_add, int &logo_pos, float *screenW, ImGuiStyle *style);

namespace dashboard
{
    void dashboard_page(int &logo_add, int &logo_pos, float *screenW, ImGuiStyle *style)
    {
        ImGuiWindow *window = ImGui::GetCurrentWindow();

        ImGui::SetNextWindowPos(globals.window_pos);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 75));
        ImGui::Begin("TopMenu", nullptr, globals.window_window_flags);

        ImGui::SetCursorPos(ImVec2(30, 10));
        ImVec4 *colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0, 0, 0, 0);
        colors[ImGuiCol_ButtonActive] = ImVec4(0, 0, 0, 0);
        if (ImGui::ImageButton(globals.back_arrow, ImVec2(24, 24)))
        {
            std::cout << "Going back\n";
            globals.logout_main_page = true;
            globals.first_page = true;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        ImGui::SetCursorPos(ImVec2(750, 14));
        if (globals.showSideMenu)
        {
            menu::RounderRetract(globals.ham_button, ImVec2(24, 24), ImVec2(8, 8), 8, IM_COL32(0, 0, 0, 0), 255);
        }
        else
        {
            menu::RoundedImage(globals.ham_button, ImVec2(24, 24), ImVec2(8, 8), 8, IM_COL32(0, 0, 0, 0), 255);
        }

        ImGui::SetCursorPos(ImVec2(400, 25));
        ImGui::Text("Dashboard top menu");

        ImGui::End();
    }
}