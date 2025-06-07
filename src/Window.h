//
// Created by tanch on 6/6/2025.
//

#ifndef WINDOW_H
#define WINDOW_H
#include <memory>

#include "Renderer.h"
#include "GLFW/glfw3.h"

class Engine;

class Window {
public:
    Window(int width, int height, const char* title);

    ~Window();

    void InitImGui();

    void InitOtherStuff();

    void InitPicking(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    std::tuple<unsigned, unsigned, unsigned> PickingPhase(Engine &engine, Renderer &renderer);

    bool GetMouseButton() const;

    void ImGuiRender();

    void SwapBuffersAndPollEvents();

    bool ShouldClose();

    int GetKey(int i);

    std::pair<double, double> GetCursorPos() const;

    void SetWindowShouldClose(bool cond);

    int GetMouseRightButton();

    GLFWwindow* window;
// private:

    int width_, height_;
    GLuint framebuffer_;
    GLuint textureColorBuffer_;
    GLuint depthTexture_;
    Shader pickingShader;
};



#endif //WINDOW_H
