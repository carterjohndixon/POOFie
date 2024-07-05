#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#else
#include <SDL2/SDL_opengl.h>
#endif

#include <stack>

namespace menu
{
    void render();
    void init(SDL_Window *window);
    void clean();
    void BackArrow(ImTextureID texture_id, const ImVec2 &size, const ImVec2 &rounding, int border_thickness, const ImU32 &border_color, int image_opacity, bool clickable, float *screenW, void (*lastPage)(float *screenW, ImGuiStyle *style));
    void RoundedImage(ImTextureID texture_id, const ImVec2 &size, const ImVec2 &rounding, int border_thickness, const ImU32 &border_color, int image_opacity, bool clickable = true);
    void RounderRetract(ImTextureID texture_id, const ImVec2 &size, const ImVec2 &rounding, int border_thickness, const ImU32 &border_color, int image_opacity, bool clickable = true);

    inline const char *window_title = "POOFie LOAD";

    // inline ImVec2 window_pos{0, 0};
    // inline ImVec2 window_size{800, 600};

    inline SDL_Window *sdl_window = nullptr;

    // inline ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;
}