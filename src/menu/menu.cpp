#include "helper.h"

void menu::render()
{
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGuiStyle &style = ImGui::GetStyle();
    static bool good_login = false;
    static bool on_first_page = false;
    static bool on_login_page = false;
    static int wait_1 = 0;
    static int wait_2 = 0;
    static bool logo_back = false;
    static int logo_pos = -70;
    static int logo_add = 0;
    static int login_missmatches = 0;
    // static int screenW = 800;
    // static int screenH = 600;

    static float screenW = 350;
    static float screenH = 450;
    static float newScreenW = 800;
    static float newScreenH = 600;

    ImGui::Begin("POOFie", &globals.active, globals.window_flags);
    {
        if (!on_first_page)
        {
            if (243 > wait_1)
            {

                gui::loading_window("Checking for updates");
            }

            if (wait_2 > 350)
                on_first_page = true;
            // on_login_page = true;
            else
                wait_2++;
        }

        if (globals.failed)
        {
            globals.tick_count++;
            if (globals.tick_count > 500)
                exit(1337);
        }

        if (globals.first_page && on_first_page && !globals.failed)
        {
            gui::connect_method_quest(&globals.connect_window_size.x, &style);
            on_login_page = true;
        }
        // Contact Point
        else if (globals.login_form && on_login_page && !globals.connection_method && !globals.failed)
        {
            gui::connect_page_contact_point(&screenW, &style);
            if (globals.connecting_to_db)
            {
                gui::loading_window("Connecting to Cassandra...");
            }
        }
        // Datastax
        else if (globals.login_form && on_login_page && globals.connection_method && !globals.failed)
        {
            gui::connect_page_datastax(&screenW, &style);
            if (globals.connecting_to_db)
            {
                gui::loading_window("Connecting to Cassandra...");
            }
        }
        else if (globals.login_loading && !globals.failed)
        {
            gui::main_page(logo_add, logo_pos, &newScreenW, &style, good_login);
        }
        else if (globals.logout_main_page && !globals.failed)
        {
            on_first_page = true;
            gui::connect_method_quest(&globals.connect_window_size.x, &style);
            on_login_page = true;
        }
    }
    ImGui::End();
}

void menu::init(SDL_Window *window)
{
    // gui::update_connection();
    sdl_window = window;
    gui::init_style();
}

void menu::clean()
{
    gui::clean();
}

void menu::BackArrow(ImTextureID texture_id, const ImVec2 &size, const ImVec2 &rounding, int border_thickness, const ImU32 &border_color, int image_opacity, bool clickable, float *screenW, void (*lastPage)(float *screenW, ImGuiStyle *style))
{
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 min = pos;
    ImVec2 max = ImVec2(pos.x + size.x, pos.y + size.y);

    ImGui::InvisibleButton("##image", size);

    if (border_thickness > 0)
    {
        draw_list->AddRect(min, max, border_color, rounding.x, ImDrawFlags_RoundCornersAll, border_thickness);
    }
    ImVec2 vector = rounding;
    draw_list->AddImageRounded(texture_id, min, max, ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, image_opacity), vector.x);

    if (clickable)
    {
        if (ImGui::IsItemClicked())
        {
            std::cout << "Going back\n";
            globals.backArrowClicked = true;
            lastPage(screenW, &ImGui::GetStyle());
            // gui::connect_method_quest(screenW, &ImGui::GetStyle());
        }
    }
}

void menu::RoundedImage(ImTextureID texture_id, const ImVec2 &size, const ImVec2 &rounding, int border_thickness, const ImU32 &border_color, int image_opacity, bool clickable)
{
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 min = pos;
    ImVec2 max = ImVec2(pos.x + size.x, pos.y + size.y);

    ImGui::InvisibleButton("##image", size);

    if (border_thickness > 0)
    {
        draw_list->AddRect(min, max, border_color, rounding.x, ImDrawFlags_RoundCornersAll, border_thickness);
    }
    ImVec2 vector = rounding;
    draw_list->AddImageRounded(texture_id, min, max, ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, image_opacity), vector.x);

    if (clickable)
    {
        static bool showText = false;
        if (ImGui::IsItemClicked())
        {
            showText = true;
            globals.showSideMenu = true;
        }

        if (showText)
            gui::hamButtonPressed();
    }
}

void menu::RounderRetract(ImTextureID texture_id, const ImVec2 &size, const ImVec2 &rounding, int border_thickness, const ImU32 &border_color, int image_opacity, bool clickable)
{
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 min = pos;
    ImVec2 max = ImVec2(pos.x + size.x, pos.y + size.y);

    ImGui::InvisibleButton("##image", size);

    if (border_thickness > 0)
    {
        draw_list->AddRect(min, max, border_color, rounding.x, ImDrawFlags_RoundCornersAll, border_thickness);
    }
    ImVec2 vector = rounding;
    draw_list->AddImageRounded(texture_id, min, max, ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, image_opacity), vector.x);

    if (clickable)
    {
        if (ImGui::IsItemClicked())
        {
            globals.showSideMenu = false;
            gui::hamButtonPressed();
        }
    }
}