#include "menu.h"
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
bool check_login_data(std::string username, std::string password);
bool Spinner(const char *label, float radius, int thickness, const ImU32 &color);

int hello_world();

void connect_to_db();
void TextCentered(std::string text, ImColor startColor, ImColor endColor, float add_x = 0, float add_y = 0, bool should_anim = false);
void DrawCircularLoadingSpinner(float x_add = 0.f, float y_add = 0.f, float current_ticks = 0.f, float max_ticks = 0.f);
static void RoundedRect(ImVec2 size, ImU32 color);
void RoundedImage(ImTextureID user_texture_id, const ImVec2 &size, const ImVec2 &rounding, int border_thickness, const ImU32 &border_color, int image_opacity);
void clear_screen();
void connect_page(int &logo_add, int &logo_pos, int *screenW, ImGuiStyle *style, bool *good_contact);
void main_page(int *logo_add, int *logo_pos, int *screenW, ImGuiStyle *style, bool *good_login, int *login_missmatches);

namespace gui
{

    bool IsInternetConnected()
    {
        // its a ghetto method but idc tbh, also needs admin rights
        int result = system("ping -c 1 google.com");
        return result == 0;
    }

    void connect_to_db(const std::string &contact_point)
    {
        DbHandler = new db::db_handler(contact_point.c_str());
    }

    bool check_db_connection()
    {
        return DbHandler->check_db_contact_point();
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

    void RoundedImage(ImTextureID user_texture_id, const ImVec2 &size, const ImVec2 &rounding, int border_thickness, const ImU32 &border_color, int image_opacity)
    {
        ImGuiWindow *window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;

        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = window->DC.CursorPos;
        ImVec2 min = pos;
        ImVec2 max = ImVec2(pos.x + size.x, pos.y + size.y);

        ImGui::InvisibleButton("##image", size);

        bool hovered = ImGui::IsItemHovered();
        bool pressed = ImGui::IsItemActive();

        // Draw border
        if (border_thickness > 0)
        {
            draw_list->AddRect(min, max, border_color, rounding.x, ImDrawFlags_RoundCornersAll, border_thickness);
        }
        ImVec2 vector = rounding;
        float x = vector.x;
        // Draw image
        draw_list->AddImageRounded(user_texture_id, min, max, ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, image_opacity), x);

        if (hovered)
        {
            // Handle hover effect
        }

        if (pressed)
        {
            // Handle click event
        }
    }

    bool check_login_data(std::string username, std::string password)
    {
        if (username == "admin" && password == "123")
        {
            return true;
        }
        else
        {
            return false;
        }
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

    void connect_page(int &logo_add, int &logo_pos, int *screenW, ImGuiStyle *style, bool *good_contact)
    {
        static bool invalid_contact = true;
        if (logo_add > -20)
            logo_add -= 1;
        else if (logo_pos < 40)
            logo_pos += 6;

        style->Colors[ImGuiCol_Text] = ImColor(171, 92, 255);
        ImGui::SetCursorPos(ImVec2((*screenW / 2) - 80, logo_pos));
        ImGui::PushFont(globals.logo_font);
        ImGui::Text("POOFie");
        ImGui::PopFont();
        style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
        ImGui::SetCursorPos(ImVec2((*screenW / 2) + 2 - 80, logo_pos + 2));
        ImGui::PushFont(globals.logo_font);
        ImGui::Text("POOFie");
        ImGui::PopFont();

        style->Colors[ImGuiCol_Button] = ImColor(171, 92, 255);         // darkest
        style->Colors[ImGuiCol_ButtonHovered] = ImColor(179, 114, 247); // light
        style->Colors[ImGuiCol_ButtonActive] = ImColor(190, 136, 247);  // lighter
        style->FrameRounding = 5.0f;

        ImGui::SetCursorPos(ImVec2((*screenW / 2) - IM_ARRAYSIZE(globals.contact_point), 150));
        ImGui::InputText("##", globals.contact_point, IM_ARRAYSIZE(globals.contact_point));

        std::string contact_point = globals.contact_point;

        ImGui::SetCursorPos(ImVec2(680, 151));
        ImGui::Image(globals.username_icon, ImVec2(18, 18));

        if (invalid_contact == false)
        {
            ImGui::PushFont(globals.verdana);
            ImGui::SetCursorPos(ImVec2((*screenW / 2) - IM_ARRAYSIZE(globals.contact_point), 175));
            ImGui::TextColored(ImVec4(1.0f, 0.f, 0.f, 1.f), "Invalid contact point!");
            ImGui::PopFont();
        }

        ImGui::SetCursorPos(ImVec2((*screenW / 2) - 130, 270));
        if (ImGui::Button("Login", ImVec2(267, 40)))
        {
            gui::connect_to_db(contact_point);
            if (gui::check_db_connection())
            {
                *good_contact = true;
                globals.login_loading = true;
                globals.main_form = true;
                globals.login_form = false;
            }
            else
            {
                invalid_contact = false;
            }
        }
    }

    void main_page(int &logo_add, int &logo_pos, int &screenW, ImGuiStyle *style, bool &good_login)
    {
        style->Colors[ImGuiCol_Text] = ImColor(171, 92, 255);
        ImGui::SetCursorPos(ImVec2((screenW / 2) - 80, logo_pos));
        ImGui::PushFont(globals.logo_font);
        ImGui::Text("POOFie");
        ImGui::PopFont();
        style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
        ImGui::SetCursorPos(ImVec2((screenW / 2) + 2 - 80, logo_pos + 2));
        ImGui::PushFont(globals.logo_font);
        ImGui::Text("POOFie");
        ImGui::PopFont();
    }
}