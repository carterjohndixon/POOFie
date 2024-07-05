#include "menu.h"
#include "FileDialog.h"
#include "SideMenu/dashboard.cpp"
#include "SideMenu/query.cpp"
#include "SideMenu/tables.cpp"
#include "../globals.h"
#include "../DB_Handler/db_handler.h"
#include <imgui/imgui_internal.h>
#include <imgui/bBlur.h>
#include <imgui/imgui.h>

#include <thread>
#include <iostream>
#include <filesystem>

static float rotation = 0.0f;
static ImVec2 spinnerCenter(0.0f, 0.0f);
static float spinnerRadius = 60.0f;
static char contact_point[255] = "";

db::db_handler *DbHandler = nullptr;

bool IsInternetConnected();
bool check_db_connection();
bool Spinner(const char *label, float radius, int thickness, const ImU32 &color);
float GetTextCursorPosX(float windowSizeX, const char *text);

int hello_world();

void ButtonHovered();
void connect_to_db_contact_point(const std::string &username, const std::string &password);
void connect_to_db_datastax(const std::string &client_id, const std::string &client_secret, const std::string &file_path);
void hamButtonPressed();
void TextCentered(std::string text, ImColor startColor, ImColor endColor, float add_x = 0, float add_y = 0, bool should_anim = false);
void DrawCircularLoadingSpinner(float x_add = 0.f, float y_add = 0.f, float current_ticks = 0.f, float max_ticks = 0.f);
static void RoundedRect(ImVec2 size, ImU32 color);
void init_style();
void connect_method_quest(float *screenW, ImGuiStyle *style, bool *good_contact);
void connect_page_contact_point(float *screenW, ImGuiStyle *style);
void connect_page_datastax(float *screenW, ImGuiStyle *style);
void main_page(int *logo_add, int *logo_pos, float *screenW, ImGuiStyle *style, bool *good_login, int *login_missmatches);

std::mutex contact_point_mutex;
std::mutex cass_user_mutex;
std::mutex cass_pass_mutex;

std::mutex cass_zip_path_mutex;
std::mutex cass_zip_file_mutex;
std::mutex cass_client_id_mutex;
std::mutex cass_client_secret_mutex;

std::atomic<bool> invalid_contact = false;

namespace gui
{

    void connect_to_db_contact_point(const std::string &contact_point, const std::string &username, const std::string &password)
    {
        DbHandler = new db::db_handler(contact_point.c_str(), username.c_str(), password.c_str());
        globals.connecting_to_db = false;
    }

    void connect_to_db_datastax(const std::string &client_id, const std::string &client_secret, const std::string &file_path)
    {
        DbHandler = new db::db_handler(client_id.c_str(), client_secret.c_str(), &file_path);
        globals.connecting_to_db = false;
    }

    bool check_db_connection()
    {
        if (DbHandler != nullptr)
            return DbHandler->is_connected();
        else
            return false;
    }

    void ButtonHovered()
    {
        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    void TextCentered(std::string text, ImColor startColor, ImColor endColor, float add_x = 0, float add_y = 0, bool should_anim = false)
    {
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

        float horizontalIndentation = (windowSize.x - textSize.x) * 0.5f;
        float verticalIndentation = (windowSize.y - textSize.y) * 0.5f;

        float minHorizontalIndentation = 20.0f;
        if (horizontalIndentation <= minHorizontalIndentation)
        {
            horizontalIndentation = minHorizontalIndentation;
        }

        float minVerticalIndentation = 20.0f;
        if (verticalIndentation <= minVerticalIndentation)
        {
            verticalIndentation = minVerticalIndentation;
        }

        ImGui::SetCursorPosY(verticalIndentation + add_y);
        ImGui::SetCursorPosX(horizontalIndentation + add_x);
        ImGui::PushTextWrapPos(windowSize.x - horizontalIndentation);
        if (should_anim)
        {
            float time = ImGui::GetTime();
            float frequency = -2.0f;

            for (int i = 0; i < text.size(); i++)
            {
                // Interpolate the color between startColor and endColor
                ImVec4 interpolatedColor;
                interpolatedColor.x = ImLerp(startColor.Value.x, endColor.Value.x, 0.5f + 0.5f * sinf(time * frequency + i * 0.3f)); // Red
                interpolatedColor.y = ImLerp(startColor.Value.y, endColor.Value.y, 0.5f + 0.5f * sinf(time * frequency + i * 0.3f)); // Green
                interpolatedColor.z = ImLerp(startColor.Value.z, endColor.Value.z, 0.5f + 0.5f * sinf(time * frequency + i * 0.3f)); // Blue
                interpolatedColor.w = 1.0f;                                                                                          // Alpha

                // Set the text color for the current letter
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(interpolatedColor.x, interpolatedColor.y, interpolatedColor.z, interpolatedColor.w));
                ImGui::TextUnformatted(text.substr(i, 1).c_str());
                ImGui::SameLine(0, 0);
                ImGui::PopStyleColor();
            }
        }
        else
        {
            ImGui::SetCursorPosX(horizontalIndentation + add_x);
            ImGui::TextWrapped(text.c_str());
        }

        ImGui::PopTextWrapPos();
    }

    void DrawCircularLoadingSpinner(float x_add = 0.f, float y_add = 0.f, float current_ticks = 0.f, float max_ticks = 0.f)
    {
        ImGuiWindow *window = ImGui::GetCurrentWindow();

        if (window == nullptr)
        {
            return;
        }

        ImGuiIO &io = ImGui::GetIO();
        ImDrawList *drawList = ImGui::GetWindowDrawList();

        spinnerCenter = ImVec2(window->Pos.x + x_add + window->Size.x * 0.5f, window->Pos.y + y_add + window->Size.y * 0.5f);

        static int def_alpha = 255;

        if (current_ticks > max_ticks)
            def_alpha--;

        const int numDots = 12;
        for (int i = 0; i < numDots; ++i)
        {
            float angle = rotation + i * (30.0f * 3.14159265359f / 180.0f);
            ImVec2 dotPos(spinnerCenter.x + cosf(angle) * spinnerRadius, spinnerCenter.y + sinf(angle) * spinnerRadius);
            float dotSize = 4.0f;
            drawList->AddCircleFilled(dotPos, dotSize, IM_COL32(171, 92, 255, def_alpha), 12);
        }

        rotation += 3.05f * io.DeltaTime;
        if (rotation > 360.0f)
            rotation -= 360.0f;
    }

    int hello_world()
    {
        const char *command = "cmd /c @echo off && title oblivion && color b && echo Hello World! && pause >NUL";
        int result = system(command);
        return true;
    }

    static void RoundedRect(ImVec2 size, ImU32 color)
    {
        ImGuiWindow *window = ImGui::GetCurrentWindow();
        ImVec2 endPos = ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y);
        window->DrawList->AddRectFilled(window->DC.CursorPos, endPos, color, 4.0f);
    }

    void VerticalTabs(const char *labels[], int labelsCount, bool showSideMenu)
    {
        ImGuiWindow *window = ImGui::GetCurrentWindow();
        if (globals.showSideMenu)
        {
            // ImGui::BringWindowToFocusFront(window);

            // ImGui::SetNextWindowPos(ImVec2(-10, 0));
            ImGui::SetNextWindowPos(ImVec2(610, 0));
            ImGui::SetNextWindowSize(ImVec2(200, ImGui::GetIO().DisplaySize.y));
            ImGui::Begin("SideMenu", nullptr, globals.window_window_flags);

            ImVec4 *colors = ImGui::GetStyle().Colors;
            colors[ImGuiCol_Button] = ImColor(69, 69, 69);
            colors[ImGuiCol_ButtonHovered] = ImColor(98, 98, 98);
            colors[ImGuiCol_ButtonActive] = ImColor(140, 140, 140);

            ImGui::GetStyle().FrameRounding = 0.0f;
            ImGui::GetStyle().WindowPadding = ImVec2(0, 0);

            // ImGui::SetCursorPos(ImVec2(40, 14));
            ImGui::SetCursorPos(ImVec2(140, 14));
            menu::RounderRetract(globals.ham_button, ImVec2(24, 24), ImVec2(8, 8), 8, IM_COL32(0, 0, 0, 0), 255);
            gui::ButtonHovered();

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 100);

            for (int i = 0; i < labelsCount; ++i)
            {
                if (i == globals.selected_button)
                {
                    colors[ImGuiCol_Button] = ImColor(140, 140, 140);
                }
                else
                {
                    colors[ImGuiCol_Button] = ImColor(69, 69, 69, 0);
                }

                if (ImGui::Button(labels[i], ImVec2(200, 45)))
                {
                    globals.currentSideTab = i;
                    globals.selected_button = i;
                }
                gui::ButtonHovered();

                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
            }

            ImGui::End();
        }
    }

    void hamButtonPressed()
    {
        ImGui::Spacing();

        const char *tabLabels[] = {"Dashboard", "Query", "Tables"};
        VerticalTabs(tabLabels, IM_ARRAYSIZE(tabLabels), true);
    }

    void noSideMenu()
    {
        VerticalTabs(nullptr, 0, false);
    }

    void init_style()
    {
        ImVec4 *colors = ImGui::GetStyle().Colors;

        ImGui::GetStyle().WindowRounding = 5.0f;
        ImGui::GetStyle().FrameRounding = 5.0f;

        colors[ImGuiCol_WindowBg].w = 1.0f;
        colors[ImGuiCol_Border].w = 0.0f;
        colors[ImGuiCol_Button] = ImColor(171, 92, 255);         // Darkest
        colors[ImGuiCol_ButtonHovered] = ImColor(179, 114, 247); // Light
        colors[ImGuiCol_ButtonActive] = ImColor(190, 136, 247);  // Lighter
    }

    bool Spinner(const char *label, float radius, int thickness, const ImU32 &color)
    {
        ImGuiWindow *window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext &g = *GImGui;
        const ImGuiStyle &style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = ImVec2(window->Pos.x + 0.f + window->Size.x * 0.5f, window->Pos.y + 0.f + window->Size.y * 0.5f);
        ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ImGui::ItemSize(bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id))
            return false;

        window->DrawList->PathClear();

        int num_segments = 30;
        int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

        const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
        const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

        const ImVec2 centre = ImVec2(pos.x, pos.y + style.FramePadding.y);

        for (int i = 0; i < num_segments; i++)
        {
            const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
            window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
                                                centre.y + ImSin(a + g.Time * 8) * radius));
        }

        window->DrawList->PathStroke(color, false, thickness);
    }

    float GetTextCursorPosX(float windowSizeX, const char *text)
    {
        auto cursorX = (windowSizeX - ImGui::CalcTextSize(text).x) * 0.5f;
        return cursorX;
    }

    void loading_window(std::string text)
    {
        ImColor startColor(108, 122, 137, 255);
        ImColor endColor(25, 122, 224, 255);
        gui::TextCentered(text, startColor, endColor, -5, 90, true);
        gui::Spinner("##loadingSpin", 60, 3, IM_COL32(108, 122, 137, 255));
    }

    void connect_method_quest(float *screenW, ImGuiStyle *style)
    {
        SDL_SetWindowSize(menu::sdl_window, globals.connect_window_size.x, globals.connect_window_size.y);
        invalid_contact = false;
        globals.currentSideTab = -1;
        globals.selected_button = -1;
        globals.showSideMenu = false;

        gui::init_style();
        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
        ImGui::SetCursorPos(ImVec2((*screenW / 2.f) - 10, 14));
        ImGui::Image(globals.poof_logo, ImVec2(24, 24));
        ImGui::SetCursorPos(ImVec2(10, 40));
        ImGui::TextWrapped("Please select which way you'll connect to your Database.");
        auto dataStaxStr = "Connect via datastax";
        auto contactPointStr = "Connect via contact point";

        ImGui::SetCursorPos(ImVec2((gui::GetTextCursorPosX(ImGui::GetWindowSize().x, dataStaxStr)), 180));
        if (ImGui::Button(dataStaxStr))
        {
            globals.connection_method = true;
            globals.login_form = true;
            globals.first_page = false;
            globals.backArrowClicked = false;
            std::cout << "Datastax Connection\n";
            std::cout << globals.connection_method << std::endl;
        }
        gui::ButtonHovered();
        ImGui::SetCursorPos(ImVec2((gui::GetTextCursorPosX(ImGui::GetWindowSize().x, contactPointStr)), 205));
        if (ImGui::Button(contactPointStr))
        {
            globals.connection_method = false;
            globals.first_page = false;
            globals.login_form = true;
            globals.backArrowClicked = false;
            std::cout << "Contact Point Connection\n";
            std::cout << globals.connection_method << std::endl;
        }
        gui::ButtonHovered();
    }

    void connect_page_contact_point(float *screenW, ImGuiStyle *style)
    {
        SDL_SetWindowSize(menu::sdl_window, globals.connect_window_size.x, globals.connect_window_size.y);
        gui::init_style();
        if (globals.backArrowClicked)
        {
            gui::connect_method_quest(screenW, &ImGui::GetStyle());
        }
        else
        {
            ImGui::SetCursorPos(ImVec2((*screenW / 2) - 10, 14));
            ImGui::Image(globals.poof_logo, ImVec2(24, 24));

            ImGui::SetCursorPos(ImVec2(10, 40));
            ImGui::TextWrapped(globals.ContactPageTextContactPoint);

            ImGui::SetCursorPos(ImVec2(30, 10));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
            if (ImGui::ImageButton(globals.back_arrow, ImVec2(24, 24)))
            {
                std::cout << "Going back\n";
                globals.backArrowClicked = true;
            }
            ImGui::PopStyleColor(3);
            gui::ButtonHovered();

            bool disable_controls = globals.connecting_to_db;
            if (disable_controls)
                ImGui::BeginDisabled(true);

            ImGui::SetCursorPos(ImVec2((*screenW / 2) - 110, 150));
            ImGui::InputText("## Contact Point", globals.contact_point, 45);
            ImGui::SetItemTooltip("Cassandra Contact Point");

            ImGui::SetCursorPos(ImVec2((*screenW / 2) - 110, 175));
            ImGui::InputText("## Username", globals.cassandra_username, 45);
            ImGui::SetItemTooltip("Cassandra Username");

            ImGui::SetCursorPos(ImVec2((*screenW / 2) - 110, 200));
            ImGui::InputText("## Password", globals.cassandra_password, 45);
            ImGui::SetItemTooltip("Cassandra Password");

            std::string contact_point;
            {
                std::lock_guard<std::mutex> lock(contact_point_mutex);
                contact_point = globals.contact_point;
            }
            std::string username;
            {
                std::lock_guard<std::mutex> lock(cass_user_mutex);
                username = globals.cassandra_username;
            }
            std::string password;
            {
                std::lock_guard<std::mutex> lock(cass_pass_mutex);
                password = globals.cassandra_password;
            }

            if (invalid_contact && !globals.field_unfilled)
            {
                ImGui::PushFont(globals.verdana);
                ImGui::SetCursorPos(ImVec2((gui::GetTextCursorPosX(ImGui::GetWindowSize().x, globals.ContactPointErr)), 225));
                ImGui::TextColored(ImVec4(1.0f, 0.f, 0.f, 1.f), globals.ContactPointErr);
                ImGui::PopFont();
            }
            else if (globals.field_unfilled)
            {
                ImGui::PushFont(globals.verdana);
                ImGui::SetCursorPos(ImVec2((gui::GetTextCursorPosX(ImGui::GetWindowSize().x, globals.NoContactPointErr)), 225));
                ImGui::TextColored(ImVec4(1.0f, 0.f, 0.f, 1.f), globals.NoContactPointErr);
                ImGui::PopFont();
            }

            ImGui::SetCursorPos(ImVec2((*screenW / 2) - 130, 270));
            if (ImGui::Button("Login", ImVec2(267, 40)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
            {
                if (contact_point.empty())
                    globals.field_unfilled = true;
                else
                    globals.field_unfilled = false;

                globals.backArrowClicked = false;

                globals.connecting_to_db = true;
                invalid_contact = false;

                std::thread([contact_point, username, password]()
                            {
            gui::connect_to_db_contact_point(contact_point, username, password);
            bool connected = gui::check_db_connection();
            if (connected) {
                globals.login_loading = true;
                globals.main_form = true;
                globals.login_form = false;
            } else {
                invalid_contact = true;
            }
            globals.connecting_to_db = false; })
                    .detach();
            }
            gui::ButtonHovered();

            if (disable_controls)
            {
                ImGui::EndDisabled();
            }
        }
    }

    void connect_page_datastax(float *screenW, ImGuiStyle *style)
    {
        gui::init_style();
        if (globals.backArrowClicked)
        {
            gui::connect_method_quest(screenW, &ImGui::GetStyle());
        }
        else
        {
            // {
            //     "clientId": "bDKOzuFUotSaTsdzKBmnlhJZ",
            //     "secret": "7ZdYRrp14QvGCYP_G8aDhBolR9+ZPw8CNfZ+bzt7QZx2QThqZ3i+Ye1htPC04n1e97P1FnBqNNdvhOp6zNwWTRWWtZTAiDml6HI7DUgc0g061fFEvWJYJChYTRlM0Af.",
            //     "token": "AstraCS:bDKOzuFUotSaTsdzKBmnlhJZ:2ba7b38a6ae80254657011658cf6767c8544ca96688a44a07d597fe6edb5e414"
            // }
            ImGui::SetCursorPos(ImVec2((*screenW / 2) - 10, 14));
            ImGui::Image(globals.poof_logo, ImVec2(24, 24));

            ImGui::SetCursorPos(ImVec2(10, 40));
            ImGui::TextWrapped(globals.ContactPageTextDatastax);

            ImGui::SetCursorPos(ImVec2(30, 10));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
            if (ImGui::ImageButton(globals.back_arrow, ImVec2(24, 24)))
            {
                std::cout << "Going back\n";
                globals.backArrowClicked = true;
            }
            ImGui::PopStyleColor(3);
            gui::ButtonHovered();

            bool disable_controls = globals.connecting_to_db;
            if (disable_controls)
                ImGui::BeginDisabled(true);

            ImGui::SetCursorPos(ImVec2(25, 150));
            ImGui::InputTextMultiline("## Client ID", globals.datastax_client_id, IM_ARRAYSIZE(globals.datastax_client_id), ImVec2(300, ImGui::GetTextLineHeight() * 1.5));
            ImGui::SetItemTooltip("Client ID");

            ImGui::SetCursorPos(ImVec2(25, 175));
            ImGui::InputTextMultiline("## Client Secret", globals.datastax_client_secret, IM_ARRAYSIZE(globals.datastax_client_secret), ImVec2(300, ImGui::GetTextLineHeight() * 1.5));
            ImGui::SetItemTooltip("Client Secret");

            ImGui::SetCursorPos(ImVec2(25, 200));
            if (ImGui::Button(globals.FilePathBrowseText))
            {
                std::cout << "Opening File Dialog\n";
                FileDialog::openFileDialog(nullptr, "../", 1, *globals.AllowedFiles, "zip files", 1);
            }
            ImGui::SetItemTooltip("Secure Connect Bundle File");
            ImGui::SameLine();
            if (globals.OpenedFileName != nullptr)
                ImGui::TextWrapped("%s", globals.OpenedFileName);

            std::string client_id;
            {
                std::lock_guard<std::mutex> lock(cass_client_id_mutex);
                client_id = globals.datastax_client_id;
            }
            std::string client_secret;
            {
                std::lock_guard<std::mutex> lock(cass_client_secret_mutex);
                client_secret = globals.datastax_client_secret;
            }
            std::string secure_path;
            {
                std::lock_guard<std::mutex> lock(cass_zip_path_mutex);
                if (globals.OpenedFilePath != nullptr)
                {
                    secure_path = globals.OpenedFilePath;
                    std::filesystem::path path(globals.OpenedFilePath);
                    if (globals.OpenedFileName != nullptr)
                        free(globals.OpenedFileName);

                    globals.OpenedFileName = strdup(path.filename().string().c_str());
                }
            }

            if (invalid_contact && !globals.field_unfilled)
            {
                ImGui::PushFont(globals.verdana);
                ImGui::SetCursorPos(ImVec2((gui::GetTextCursorPosX(ImGui::GetWindowSize().x, globals.ContactPointErr)), 225));
                ImGui::TextColored(ImVec4(1.0f, 0.f, 0.f, 1.f), globals.ContactPointErr);
                ImGui::PopFont();
            }
            else if (globals.field_unfilled)
            {
                ImGui::PushFont(globals.verdana);
                ImGui::SetCursorPos(ImVec2((gui::GetTextCursorPosX(ImGui::GetWindowSize().x, globals.CredsProvidedErr)), 225));
                ImGui::TextColored(ImVec4(1.0f, 0.f, 0.f, 1.f), globals.CredsProvidedErr);
                ImGui::PopFont();
            }

            ImGui::SetCursorPos(ImVec2((*screenW / 2) - 130, 270));
            if (ImGui::Button("Login", ImVec2(267, 40)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
            {
                if (secure_path.empty())
                    globals.field_unfilled = true;
                else
                    globals.field_unfilled = false;

                globals.backArrowClicked = false;

                globals.connecting_to_db = true;
                invalid_contact = false;

                std::thread([client_id, client_secret, secure_path]()
                            {
            std::cout << client_id << std::endl;
            std::cout << client_secret << std::endl;
            gui::connect_to_db_datastax(client_id, client_secret, secure_path);
            if(!globals.ConnectionErr) {
            bool connected = gui::check_db_connection();
            if (connected) {
                globals.login_loading = true;
                globals.main_form = true;
                globals.login_form = false;
            } else {
                invalid_contact = true;
            }
            }
            globals.connecting_to_db = false; })
                    .detach();
            }
            gui::ButtonHovered();

            if (disable_controls)
            {
                ImGui::EndDisabled();
            }
        }
    }

    void main_page(int &logo_add, int &logo_pos, float *screenW, ImGuiStyle *style, bool &good_login)
    {
        SDL_SetWindowSize(menu::sdl_window, globals.window_size.x, globals.window_size.y);
        // 127.0.0.1

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
        gui::ButtonHovered();

        ImGui::SetCursorPos(ImVec2(750, 14));
        menu::RoundedImage(globals.ham_button, ImVec2(24, 24), ImVec2(8, 8), 8, IM_COL32(0, 0, 0, 0), 255);
        gui::ButtonHovered();

        ImGuiWindow *window = ImGui::GetCurrentWindow();
        if (globals.showSideMenu)
        {
            ImGui::BringWindowToDisplayBack(window);
        }

        if (globals.currentSideTab == 0)
        {
            // Dashboard
            dashboard::dashboard_page(logo_add, logo_pos, screenW, &ImGui::GetStyle());
        }
        if (globals.currentSideTab == 1)
        {
            // Query
            query::query_page(logo_add, logo_pos, screenW, &ImGui::GetStyle());
        }
        if (globals.currentSideTab == 2)
        {
            // Tables
            tables::tables_page(logo_add, logo_pos, screenW, &ImGui::GetStyle());
        }
    }

    void clean()
    {
        delete DbHandler;
    }
}