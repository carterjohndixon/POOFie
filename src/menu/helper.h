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

void connect_to_db(const std::string &username, const std::string &password);
void hamButtonPressed();
void TextCentered(std::string text, ImColor startColor, ImColor endColor, float add_x = 0, float add_y = 0, bool should_anim = false);
void DrawCircularLoadingSpinner(float x_add = 0.f, float y_add = 0.f, float current_ticks = 0.f, float max_ticks = 0.f);
static void RoundedRect(ImVec2 size, ImU32 color);
void init_style();
void connect_method_quest(float *screenW, ImGuiStyle *style, bool *good_contact);
void connect_page_contact_point(float *screenW, ImGuiStyle *style);
void main_page(int *logo_add, int *logo_pos, int *screenW, ImGuiStyle *style, bool *good_login, int *login_missmatches);

std::mutex contact_point_mutex;
std::mutex cass_user_mutex;
std::mutex cass_pass_mutex;
std::mutex cass_zip_path_mutex;
std::atomic<bool> invalid_contact = true;

namespace gui
{

    bool IsInternetConnected()
    {
        // its a ghetto method but idc tbh, also needs admin rights
        int result = system("ping -c 1 google.com");
        return result == 0;
    }

    void connect_to_db(const std::string &contact_point)
    // void connect_to_db(const std::string &username, const std::string &password)
    {
        DbHandler = new db::db_handler(contact_point.c_str());
        // DbHandler = new db::db_handler(username.c_str(), password.c_str());
        globals.connecting_to_db = false;
    }

    bool check_db_connection()
    {
        if (DbHandler != nullptr)
            return DbHandler->is_connected();
        else
            return false;
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

        // Calculate the spinner position (centered)
        spinnerCenter = ImVec2(window->Pos.x + x_add + window->Size.x * 0.5f, window->Pos.y + y_add + window->Size.y * 0.5f);
        // ImVec2 spinnerCenter(io.DisplaySize.x * 0.5f + x_add, io.DisplaySize.y * 0.5f + y_add);

        static int def_alpha = 255;

        if (current_ticks > max_ticks)
            def_alpha--;

        // Draw a semi-transparent background circle

        // Draw the rotating circular loading spinner as dots
        const int numDots = 12; // Number of dots in the spinner
        for (int i = 0; i < numDots; ++i)
        {
            float angle = rotation + i * (30.0f * 3.14159265359f / 180.0f);
            ImVec2 dotPos(spinnerCenter.x + cosf(angle) * spinnerRadius, spinnerCenter.y + sinf(angle) * spinnerRadius);
            float dotSize = 4.0f; // Adjust the size of the dots
            drawList->AddCircleFilled(dotPos, dotSize, IM_COL32(171, 92, 255, def_alpha), 12);
        }

        // Adjust the rotation speed by changing the value below
        rotation += 3.05f * io.DeltaTime; // Adjust the multiplier (0.05f) for speed
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
        // window->DrawList->AddRectFilled(window->DC.CursorPos, window->DC.CursorPos + size, color, 4.0f);
    }

    // void VerticalTabs(const char *labels[], int labelsCount, int *selectedTab, bool showSideMenu)
    void VerticalTabs(const char *labels[], int labelsCount, bool showSideMenu)
    {
        static int logo_pos = -70;
        static int logo_add = 0;
        static int screenW = 800;
        static int selected_button = -1;

        ImGuiWindow *window = ImGui::GetCurrentWindow();
        if (globals.showSideMenu)
        {
            ImGui::BringWindowToFocusFront(window);

            ImGui::SetNextWindowPos(ImVec2(-10, 0));
            ImGui::SetNextWindowSize(ImVec2(200, ImGui::GetIO().DisplaySize.y));
            ImGui::Begin("SideMenu", nullptr, globals.window_window_flags);

            ImVec4 *colors = ImGui::GetStyle().Colors;
            // colors[ImGuiCol_WindowBg].w = 1.0f;
            colors[ImGuiCol_Button] = ImColor(69, 69, 69);
            colors[ImGuiCol_ButtonHovered] = ImColor(98, 98, 98);
            colors[ImGuiCol_ButtonActive] = ImColor(140, 140, 140);

            ImGui::GetStyle().FrameRounding = 0.0f;
            ImGui::GetStyle().WindowPadding = ImVec2(0, 0);

            ImGui::SetCursorPos(ImVec2(40, 30));
            menu::RounderRetract(globals.ham_button, ImVec2(24, 24), ImVec2(8, 8), 8, IM_COL32(0, 0, 0, 0), 255);

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 100);

            for (int i = 0; i < labelsCount; ++i)
            {
                if (i == selected_button)
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
                    selected_button = i;
                }

                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
            }

            ImGui::End();
        }
    }

    void hamButtonPressed()
    {
        ImGui::Spacing();

        const char *tabLabels[] = {"Dashboard", "Query", "Tables"};
        // static int selectedTab = 0;

        // VerticalTabs(tabLabels, IM_ARRAYSIZE(tabLabels), &selectedTab, true);
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
        colors[ImGuiCol_Button] = ImColor(171, 92, 255);
        colors[ImGuiCol_ButtonHovered] = ImColor(179, 114, 247);
        colors[ImGuiCol_ButtonActive] = ImColor(190, 136, 247);

        // style->Colors[ImGuiCol_Button] = ImColor(171, 92, 255);         // darkest
        // style->Colors[ImGuiCol_ButtonHovered] = ImColor(179, 114, 247); // light
        // style->Colors[ImGuiCol_ButtonActive] = ImColor(190, 136, 247);  // lighter
        // style->FrameRounding = 5.0f;
    }

    bool Spinner(const char *label, float radius, int thickness, const ImU32 &color)
    {
        ImGuiWindow *window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext &g = *GImGui;
        const ImGuiStyle &style = g.Style;
        const ImGuiID id = window->GetID(label);

        // ImVec2 pos = window->DC.CursorPos;
        ImVec2 pos = ImVec2(window->Pos.x + 0.f + window->Size.x * 0.5f, window->Pos.y + 0.f + window->Size.y * 0.5f);
        ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ImGui::ItemSize(bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id))
            return false;

        // Render
        window->DrawList->PathClear();

        int num_segments = 30;
        int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

        const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
        const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

        // const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);
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
        ImGui::SetCursorPos(ImVec2((*screenW / 2.f) - 10, 14));
        ImGui::Image(globals.poof_logo, ImVec2(24, 24));
        ImGui::TextWrapped("Please select which way you'll connect to your Database.");
        auto dataStaxStr = "Connect via datastax";
        auto contactPointStr = "Connect via contact point";

        ImGui::SetItemTooltip("Secure Connection Bundle Path");
        ImGui::SetCursorPos(ImVec2((gui::GetTextCursorPosX(ImGui::GetWindowSize().x, dataStaxStr)), 180));
        if (ImGui::Button(dataStaxStr))
        {
            globals.connection_method, globals.login_form = true;
            globals.first_page = false;
            std::cout << "Datastax Connection\n";
            std::cout << globals.connection_method << std::endl;
        }
        ImGui::SetCursorPos(ImVec2((gui::GetTextCursorPosX(ImGui::GetWindowSize().x, contactPointStr)), 205));
        if (ImGui::Button(contactPointStr))
        {
            globals.connection_method, globals.first_page = false;
            globals.login_form = true;
            std::cout << "Contact Point Connection\n";
            std::cout << globals.connection_method << std::endl;
        }
    }

    void connect_page_contact_point(float *screenW, ImGuiStyle *style)
    {

        ImGui::SetCursorPos(ImVec2((*screenW / 2) - 10, 14));
        ImGui::Image(globals.poof_logo, ImVec2(24, 24));

        ImGui::SetCursorPos(ImVec2(30, 14));
        menu::BackArrow(globals.back_arrow, ImVec2(24, 24), ImVec2(8, 8), 8, IM_COL32(0, 0, 0, 0), 255, true, &globals.connect_window_size.x, gui::connect_method_quest);
        // menu::BackArrow(globals.back_arrow, ImVec2(24, 24), ImVec2(8, 8), 8, IM_COL32(0, 0, 0, 0), 255, true, &globals.connect_window_size.x, gui::connect_method_quest(screenW, &ImGui::GetStyle()));

        bool disable_controls = globals.connecting_to_db;
        if (disable_controls)
            ImGui::BeginDisabled(true);

        // ImGui::TextWrapped("Please enter your Client ID, Secret, and upload the Secure Connection Bundle.", ImGuiInputTextFlags_Multiline);

        ImGui::SetCursorPos(ImVec2((*screenW / 2) - 110, 150));
        ImGui::InputText("## Contact Point", globals.contact_point, 45);
        ImGui::SetItemTooltip("Cassandra Contact Point");

        // ImGui::PushItemWidth(-1);
        // ImGui::InputText("##Client ID", globals.cassandra_username, IM_ARRAYSIZE(globals.cassandra_username));
        // ImGui::SetItemTooltip("Client ID");
        // // ImGui::InputText("Client Secret", globals.cassandra_password, IM_ARRAYSIZE(globals.cassandra_password), ImGuiInputTextFlags_Password | ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_AutoSelectAll);
        // ImGui::InputTextMultiline("##Client Secret", globals.cassandra_password, IM_ARRAYSIZE(globals.cassandra_password), ImVec2(0, ImGui::GetTextLineHeight() * 1.5));
        // ImGui::SetItemTooltip("Client Secret");
        // ImGui::PopItemWidth();

        // if (ImGui::Button("Choose Secure Connection Bundle"))
        // {
        //     std::cout << "Opening File Dialog\n";
        // }
        // ImGui::SameLine();
        // ImGui::TextWrapped("%s", globals.OpenedFileName);

        // ImGui::SetCursorPos(ImVec2((*screenW / 2) - 110, 210));
        // ImGui::InputText("## Bundle Path", globals.OpenedFileName, 45);
        // ImGui::SetItemTooltip("Secure Connection Bundle Path");
        // ImGui::SetCursorPos(ImVec2(240, 210));
        // if (ImGui::Button("Browse..."))
        // {
        //     // fileDialog.Open();
        //     std::cout << "Opening File Dialog\n";
        // }

        // std::string username;
        // {
        //     std::lock_guard<std::mutex> lock(cass_user_mutex);
        //     username = globals.cassandra_username;
        // }
        // std::string password;
        // {
        //     std::lock_guard<std::mutex> lock(cass_pass_mutex);
        //     password = globals.cassandra_password;
        // }
        // std::string zip_path;
        // {
        //     std::lock_guard<std::mutex> lock(cass_zip_path_mutex);
        //     zip_path = globals.OpenedFileName;
        // }
        std::string contact_point;
        {
            std::lock_guard<std::mutex> lock(contact_point_mutex);
            contact_point = globals.contact_point;
        }

        // ImGui::SetCursorPos(ImVec2(680, 151));
        // ImGui::Image(globals.username_icon, ImVec2(18, 18));

        if (!invalid_contact)
        {
            ImGui::PushFont(globals.verdana);
            ImGui::SetCursorPos(ImVec2((IM_ARRAYSIZE(globals.contact_point) / 2) - 60, 175));
            ImGui::TextColored(ImVec4(1.0f, 0.f, 0.f, 1.f), "Invalid contact point!");
            ImGui::PopFont();
        }

        ImGui::SetCursorPos(ImVec2((*screenW / 2) - 130, 270));
        if (ImGui::Button("Login", ImVec2(267, 40)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
        {
            // std::string captured_username;
            // {
            //     std::lock_guard<std::mutex> lock(cass_user_mutex);
            //     captured_username = globals.cassandra_username;
            // }
            // std::string captured_password;
            // {
            //     std::lock_guard<std::mutex> lock(cass_pass_mutex);
            //     captured_password = globals.cassandra_password;
            // }

            std::string captured_contact_point;
            {
                std::lock_guard<std::mutex> lock(contact_point_mutex);
                captured_contact_point = globals.contact_point;
            }

            globals.connecting_to_db = true;
            invalid_contact = true;

            std::thread([captured_contact_point]()
                        {
            gui::connect_to_db(captured_contact_point);
            // gui::connect_to_db(captured_username, captured_password);
            bool connected = gui::check_db_connection();
            if (connected) {
                // *good_contact = true;
                globals.login_loading = true;
                globals.main_form = true;
                globals.login_form = false;
            } else {
                invalid_contact = false;
            }
            globals.connecting_to_db = false; })
                .detach();
        }

        if (disable_controls)
        {
            ImGui::EndDisabled();
        }
    }

    void main_page(int &logo_add, int &logo_pos, int *screenW, ImGuiStyle *style, bool &good_login)
    {
        SDL_SetWindowSize(menu::sdl_window, globals.window_size.x, globals.window_size.y);

        ImGui::SetCursorPos(ImVec2(30, 30));
        menu::RoundedImage(globals.ham_button, ImVec2(24, 24), ImVec2(8, 8), 8, IM_COL32(0, 0, 0, 0), 255);

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