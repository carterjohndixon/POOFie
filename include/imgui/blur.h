#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <math.h>

// #include <d3dx9.h>
// #include <d3d9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <SDL2/SDL.h>

extern void DrawBackgroundBlur(ImDrawList *drawList);