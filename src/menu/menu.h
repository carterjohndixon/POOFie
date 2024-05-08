#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#else
#include <SDL2/SDL_opengl.h>
#endif

namespace menu
{
    void render(SDL_Window *window);
    void init(SDL_Window *window);

    inline const char *window_title = "POOFie LOAD";

    inline ImVec2 screen_res{000, 000};
    inline ImVec2 window_pos{0, 0};
    inline ImVec2 window_size{800, 600};

    inline ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;
    // inline ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoSavedSettings;
}