// #include "menu.h"
// #include "../globals.h"

// #include <imgui/imgui_internal.h>
// #include <imgui/bBlur.h>

// #include <thread>
// #include <iostream>

// std::string key_input = "";

#include "helper.h"

void menu::render(SDL_Window *window)
{
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGuiStyle &style = ImGui::GetStyle();
    static bool good_login = false;
    static bool on_login_page = false;
    static int wait_1 = 0;
    static int wait_2 = 0;
    static bool logo_back = false;
    static int logo_pos = -70;
    static int logo_add = 0;
    static int login_missmatches = 0;
    static int screenW = 800;
    static int screenH = 600;

    ImGui::Begin("POOFie", &globals.active, window_flags);
    {
        if (!logo_back)
        {
            ImGui::SetCursorPos(ImVec2(800 / 2 - 10, 8));
            ImGui::Image(globals.poof_logo, ImVec2(24, 24));
        }
        if (!on_login_page)
        {
            if (243 > wait_1)
            {

                ImColor startColor(108, 122, 137, 255);
                ImColor endColor(25, 122, 224, 255);
                gui::TextCentered("Checking for updates", startColor, endColor, -5, 90, true);
                gui::Spinner("##loadingSpin", 60, 3, IM_COL32(108, 122, 137, 255));
            }

            if (wait_2 > 350)
                on_login_page = true;
            else
                wait_2++;
        }
        if (!globals.checked_connection)
        {
            if (!gui::IsInternetConnected())
            {
                globals.connection_message = "(Connected)";
            }
            else
            {
                globals.connection_message = "(Failed to check connection)";
                globals.fail_message = "Failed to check connection";
                globals.failed = true;
            }
            globals.checked_connection = true;
        }

        if (globals.failed)
        {
            globals.tick_count++;
            if (globals.tick_count > 500)
                exit(1337);
        }

        if (globals.login_form && on_login_page && !globals.failed)
        {
            if (logo_add > -20)
                logo_add -= 1;
            else if (logo_pos < 40)
                logo_pos += 6;

            style.Colors[ImGuiCol_Text] = ImColor(171, 92, 255);
            ImGui::SetCursorPos(ImVec2((screenW / 2) - 80, logo_pos));
            ImGui::PushFont(globals.logo_font);
            ImGui::Text("POOFie");
            ImGui::PopFont();
            style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
            ImGui::SetCursorPos(ImVec2((screenW / 2) + 2 - 80, logo_pos + 2));
            ImGui::PushFont(globals.logo_font);
            ImGui::Text("POOFie");
            ImGui::PopFont();

            style.Colors[ImGuiCol_Button] = ImColor(171, 92, 255);
            style.Colors[ImGuiCol_ButtonHovered] = ImColor(171, 92, 255);
            style.Colors[ImGuiCol_ButtonActive] = ImColor(171, 92, 255);
            style.FrameRounding = 5.0f;

            ImGui::SetCursorPos(ImVec2((screenW / 2) - IM_ARRAYSIZE(globals.username), 150));
            ImGui::InputText("##", globals.username, IM_ARRAYSIZE(globals.username));

            std::string user_name = globals.username;

            ImGui::SetCursorPos(ImVec2(680, 151));
            ImGui::Image(globals.username_icon, ImVec2(18, 18));

            ImGui::SetCursorPos(ImVec2((screenW / 2) - IM_ARRAYSIZE(globals.password), 210));
            ImGui::InputText(" ", globals.password, IM_ARRAYSIZE(globals.password), ImGuiInputTextFlags_Password);

            std::string pass_word = globals.password;

            ImGui::SetCursorPos(ImVec2(680, 211));
            ImGui::Image(globals.password_icon, ImVec2(18, 18));

            ImGui::SetCursorPos(ImVec2((screenW / 2) - 130, 270));
            if (ImGui::Button("Login", ImVec2(267, 40)))
            {
                if (gui::check_login_data(user_name, pass_word))
                    good_login = true;
                else
                    login_missmatches++;
            }
        }
    }
    ImGui::End();
}