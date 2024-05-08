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
    ImTextureID password_icon = nullptr;
    ImTextureID cs2_icon = nullptr;
    ImTextureID poof_logo = nullptr;

    bool active = true;
    char username[255] = "";
    char password[255] = "";
    bool login_form = true;
    bool login_loading = false;
    bool main_form = false;
    bool loading_product = false;
    bool loaded_product = false;
    bool failed = false;
    bool checked_connection = true;
    bool initialized_new_window_name = false;
    bool loaded_successfull = false;
    std::string fail_message = "";
    std::string connection_message = "";
    std::string status = "unknown";
    std::string version = "0.0.1";
    std::string last_uptade = "29.09.2023";
    int final_selected_product = 0;
    int tick_count = 0;
    int tick_count2 = 0;
    ImFont *verdana;
    ImFont *logo_font;
};

extern c_globals globals;