#define STB_IMAGE_IMPLEMENTATION
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <OpenGL/gl.h>
#include <stb/stb_image.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
// #if defined(IMGUI_IMPL_OPENGL_ES2)
// #else
#include <SDL2/SDL_opengl.h>
// #endif

#include "menu/menu.h"
#include "bytes/test.h"
#include "globals.h"

#include <iostream>

// static int WindowWidth = 175;
// static int WindowHeight = 225;
static int WindowWidth = 800;
static int WindowHeight = 600;

const char *cassandra_icon = "assets/cassandra.png";

c_globals globals;

SDL_HitTestResult HitTestCallback(SDL_Window *window, const SDL_Point *point, void *data)
{
    if (point->y <= 30)
        return SDL_HITTEST_DRAGGABLE;
    else
        return SDL_HITTEST_NORMAL;
}

GLuint LoadTextureFromFileInMemory(unsigned char *data, int size)
{
    int width, height, channels;
    unsigned char *image = stbi_load_from_memory(data, size, &width, &height, &channels, STBI_rgb_alpha);
    if (!image)
    {
        fprintf(stderr, "Failed to load image from memory\n");
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(image);

    return textureID;
}

int main(int, char **)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
#if defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char *glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

    // SDL_Window *window = SDL_CreateWindow("Cassandra", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALLOW_HIGHDPI | ImGuiWindowFlags_NoResize); // borderless
    SDL_Window *window = SDL_CreateWindow("Cassandra", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | ImGuiWindowFlags_NoResize); // with border
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    SDL_Surface *cassandra_surface = IMG_Load(cassandra_icon);
    if (!cassandra_surface)
    {
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowIcon(window, cassandra_surface);

    // Initialize ImGui with SDL2 and OpenGL3
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    if (globals.username_icon == nullptr)
    {
        GLuint textureID = LoadTextureFromFileInMemory(user, 14903);
        globals.username_icon = (void *)(intptr_t)textureID;
    }
    if (globals.poof_logo == nullptr)
    {
        GLuint textureID = LoadTextureFromFileInMemory(poof_logo, 5004);
        globals.poof_logo = (void *)(intptr_t)textureID;
    }

    globals.verdana = io.Fonts->AddFontFromFileTTF("../sdl-opengl/fonts/Verdana.ttf", 15);
    globals.logo_font = io.Fonts->AddFontFromFileTTF("../sdl-opengl/fonts/verdanab.ttf", 53);

    menu::init();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    SDL_SetWindowHitTest(window, HitTestCallback, NULL);

    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        int guiWidth, guiHeight;
        SDL_GetWindowSize(window, &guiWidth, &guiHeight);

        ImVec2 centerPoint(guiWidth * 0.5f, guiHeight * 0.5);
        ImVec2 windowSize(WindowWidth, WindowHeight);
        ImVec2 windowPos(centerPoint.x - windowSize.x * 0.5f, centerPoint.y - windowSize.y * 0.5f);

        // Set ImGui window position
        ImGui::SetNextWindowPos(windowPos);

        // Set ImGui window size to match SDL window size
        ImGui::SetNextWindowSize(windowSize);
        {
            menu::render(window);
        }

        glClearColor(0.108f, 0.122f, 0.137f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    menu::clean();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_FreeSurface(cassandra_surface);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}