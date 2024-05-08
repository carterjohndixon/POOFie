#include <imgui/blur.h>
#include <imgui/blur_binary.h>

#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

static GLuint rtBackup = 0;
static GLuint blurShaderX = 0;
static GLuint blurShaderY = 0;
static GLuint blurTexture = 0;
static int backbufferWidth = 0;
static int backbufferHeight = 0;

GLuint loadShader(const char *vertexShaderSource, const char *fragmentShaderSource)
{
    // Create shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile vertex shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Compile fragment shader
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Create shader program
    GLuint shaderProgram = glCreateProgram();

    // Attach shaders to program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link program
    glLinkProgram(shaderProgram);

    // Clean up shaders (they are already attached to the program)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Placeholder vertex shader source
const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 position;
    void main()
    {
        gl_Position = vec4(position, 0.0, 1.0);
    }
)";

// Placeholder fragment shader source for horizontal blur
const char *fragmentShaderXSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main()
    {
        // Placeholder fragment shader code for horizontal blur
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color for demonstration
    }
)";

// Placeholder fragment shader source for vertical blur
const char *fragmentShaderYSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main()
    {
        // Placeholder fragment shader code for vertical blur
        FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Green color for demonstration
    }
)";

static void loadShadersIfNeeded()
{
    if (!blurShaderX)
    {
        blurShaderX = loadShader(vertexShaderSource, fragmentShaderXSource);
    }

    if (!blurShaderY)
    {
        blurShaderY = loadShader(vertexShaderSource, fragmentShaderYSource);
    }
}

static void BeginBlur(const ImDrawList *parent_list, const ImDrawCmd *cmd)
{
    SDL_GLContext glContext = SDL_GL_GetCurrentContext();
    if (!glContext)
        return;

    loadShadersIfNeeded();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int width = viewport[2];
    int height = viewport[3];

    if (backbufferWidth != width || backbufferHeight != height)
    {
        if (blurTexture != 0)
        {
            glDeleteTextures(1, &blurTexture);
        }

        glGenTextures(1, &blurTexture);
        glBindTexture(GL_TEXTURE_2D, blurTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        backbufferWidth = width;
        backbufferHeight = height;
    }

    glBindTexture(GL_TEXTURE_2D, blurTexture);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture, 0);

    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, width, height, 0);

    glViewport(0, 0, width, height);
}

static void FirstBlurPass(const ImDrawList *parent_list, const ImDrawCmd *cmd)
{
    SDL_GLContext glContext = SDL_GL_GetCurrentContext();
    if (!glContext)
        return;

    glUseProgram(blurShaderX);

    float params[4] = {1.0f / backbufferWidth};
    glUniform1fv(glGetUniformLocation(blurShaderX, "blurSize"), 1, params);
}

static void SecondBlurPass(const ImDrawList *parent_list, const ImDrawCmd *cmd)
{
    SDL_GLContext glContext = SDL_GL_GetCurrentContext();
    if (!glContext)
        return;

    glUseProgram(blurShaderY);

    float params[4] = {1.0f / backbufferHeight};
    glUniform1fv(glGetUniformLocation(blurShaderY, "blurSize"), 1, params);
}

static void EndBlur(const ImDrawList *parent_list, const ImDrawCmd *cmd)
{
    SDL_GLContext glContext = SDL_GL_GetCurrentContext();
    if (!glContext)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

inline void DrawBackgroundBlur(ImDrawList *drawList)
{
    drawList->AddCallback(BeginBlur, nullptr);

    for (int i = 0; i < 8; ++i)
    {
        drawList->AddCallback(FirstBlurPass, nullptr);
        drawList->AddImage((void *)(intptr_t)blurTexture, ImVec2(0.0f, 0.0f), ImVec2(backbufferWidth * 1.0f, backbufferHeight * 1.0f));
        drawList->AddCallback(SecondBlurPass, nullptr);
        drawList->AddImage((void *)(intptr_t)blurTexture, ImVec2(0.0f, 0.0f), ImVec2(backbufferWidth * 1.0f, backbufferHeight * 1.0f));
    }

    drawList->AddCallback(EndBlur, nullptr);
    drawList->AddImageRounded((void *)(intptr_t)blurTexture, ImVec2(0.0f, 0.0f), ImVec2(backbufferWidth * 1.0f, backbufferHeight * 1.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), IM_COL32(255, 255, 255, 255), 7.f);
}