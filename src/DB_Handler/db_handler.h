#pragma once

#include <cassandra.h>
// #include <imgui/imgui.h>
// #include <imgui/imgui_impl_sdl2.h>
// #include <imgui/imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#else
#include <SDL2/SDL_opengl.h>
#endif

#include <cstdint>
#include <functional>

namespace db
{
    class db_handler
    {
    public:
        db_handler(const char *contact_points);
        // db_handler(const char *username, const char *password);
        ~db_handler();

        bool is_connected();

    private:
        CassCluster *cluster;
        CassSession *session;
        CassError *rc;

        const char *contact_points;
    };
}