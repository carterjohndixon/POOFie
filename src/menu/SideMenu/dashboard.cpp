#include "../../globals.h"
#include "../../DB_Handler/db_handler.h"
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>

#include <iostream>

void dashboard_page(int &logo_add, int &logo_pos, int *screenW, ImGuiStyle *style);

namespace dashboard
{
    void dashboard_page(int &logo_add, int &logo_pos, int *screenW, ImGuiStyle *style)
    {
        ImGuiWindow *window = ImGui::GetCurrentWindow();

        ImGui::SetNextWindowPos(globals.window_pos);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 75));
        ImGui::Begin("TopMenu", nullptr, globals.window_window_flags);

        ImGui::SetCursorPos(ImVec2(30, 30));
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