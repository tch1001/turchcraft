#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <format>
#include <chrono>

#include "Cube.h"
#include "Engine.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

void framebuffer_size_callback_2(GLFWwindow *window, int width, int height) {
    GLCall(glViewport(0, 0, width, height));
}
int main() {
    const std::string vertexShaderPath = std::format("{}/res/shaders/Vertex.shader", PROJECT_SOURCE_DIR);
    const std::string vertexShaderPath_2 = std::format("{}/res/shaders/Vertex.shader", PROJECT_SOURCE_DIR);
    const std::string fragmentShaderPath = std::format("{}/res/shaders/Fragment.shader", PROJECT_SOURCE_DIR);
    const std::string pickingFragmentShaderPath = std::format("{}/res/shaders/PickingFragment.shader", PROJECT_SOURCE_DIR);
    Window window{WINDOW_WIDTH, WINDOW_HEIGHT, "TurchCraft"};
    window.InitImGui();
    window.InitOtherStuff();
    window.InitPicking(vertexShaderPath, pickingFragmentShaderPath);

    Engine::Config config{
        .vertexShaderPath = vertexShaderPath_2,
        .graphicsFragmentShaderPath = fragmentShaderPath,
        .windowWidth = WINDOW_WIDTH,
        .windowHeight = WINDOW_HEIGHT,
    };
    Engine engine{config};

    {
        // Texture texture(std::format("{}/res/textures/texture.png", PROJECT_SOURCE_DIR));
        // shader.Bind();
        // texture.Bind();
        // shader.SetUniform1i("u_Texture", 0);

        Renderer renderer;

        Cube cube{}, cube2{};
        cube.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        cube.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));

        cube2.SetPosition(glm::vec3(0.0f, 100.0f, 0.0f));
        cube2.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));

        engine.AddObject(std::move(cube));
        engine.AddObject(std::move(cube2));

        // Render loop
        while (!window.ShouldClose()) {
            glfwPollEvents(); // Process events

            window.PickingPhase(engine, renderer);
            engine.ProcessInputs(window);
            engine.UserAction(window);
            engine.RenderObjects(renderer);

            renderer.Clear();

            window.ImGuiRender();
            window.SwapBuffersAndPollEvents();
        }
    }

    return 0;
}
