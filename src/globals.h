#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#else
#include <SDL2/SDL_opengl.h>
#endif

#include <string>

class c_globals
{
public:
    ImTextureID oblivion_icon = nullptr;
    ImTextureID username_icon = nullptr;
    // ImTextureID password_icon = nullptr;
    // ImTextureID cs2_icon = nullptr;
    ImTextureID poof_logo = nullptr;
    ImTextureID ham_button = nullptr;
    ImTextureID circle_x = nullptr;
    ImTextureID back_arrow = nullptr;
    ImTextureID logout = nullptr;

    ImVec2 window_pos{0, 0};
    ImVec2 connect_window_size{350, 450};
    ImVec2 window_size{800, 600};

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;
    ImGuiWindowFlags window_window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;

    char const *AllowedFiles[2] = {"*.zip"};
    char *OpenedFilePath = nullptr;
    char *OpenedFileName = nullptr;

    // true: Datastax
    // false: Contact Point
    bool connection_method = false;

    bool backArrowClicked = false;
    bool logout_main_page = false;

    const char *ContactPointErr = "Information provided is incorrect!";
    const char *NoContactPointErr = "Contact point must be provided!";
    const char *CredsProvidedErr = "Credentials must be provided!";
    const char *ContactPageTextContactPoint = "Please input the contact point to your database. Username and Password is optional based on connection options.";
    const char *ContactPageTextDatastax = "Please input your Client ID, Client Secret, along with the path to your secure-connect zip file.";
    const char *FilePathBrowseText = "Choose File";

    bool active = true;
    bool showSideMenu = false;
    bool showText = false;
    bool connecting_to_db = false;
    bool field_unfilled = false;

    char contact_point[255] = "";
    char cassandra_username[510] = "";
    char cassandra_password[510] = "";

    char datastax_client_id[510] = "";
    char datastax_client_secret[510] = "";

    bool ConnectionErr = false;
    std::string ConnectionErrStr = "";

    bool first_page = true;
    bool login_form = false;
    bool login_loading = false;
    bool main_form = false;
    bool failed = false;
    std::string fail_message = "";
    std::string connection_message = "";
    int tick_count = 0;
    int currentSideTab = -1;
    int selected_button = -1;
    ImFont *verdana;
    ImFont *logo_font;
};

extern c_globals globals;