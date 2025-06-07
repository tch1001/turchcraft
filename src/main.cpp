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
    const std::string fragmentShaderPath = std::format("{}/res/shaders/Fragment.shader", PROJECT_SOURCE_DIR);
    const std::string pickingFragmentShaderPath = std::format("{}/res/shaders/PickingFragment.shader", PROJECT_SOURCE_DIR);
    Window window{WINDOW_WIDTH, WINDOW_HEIGHT, "TurchCraft"};
    window.InitImGui();
    window.InitOtherStuff();
    window.InitPicking(vertexShaderPath, pickingFragmentShaderPath);

    Engine::Config config{
        .vertexShaderPath = vertexShaderPath,
        .graphicsFragmentShaderPath = fragmentShaderPath,
        .windowWidth = WINDOW_WIDTH,
        .windowHeight = WINDOW_HEIGHT,
    };
    Engine engine{config};

    {
        // Texture texture(std::format("{}/res/textures/texture.png", PROJECT_SOURCE_DIR));
        // engine.graphicsShader.Bind();
        // texture.Bind();
        // engine.graphicsShader.SetUniform1i("u_Texture", 0);
        //
        Renderer renderer;

        Cube cube{};
        cube.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        cube.SetRotation(glm::vec3(0.0f, 30.0f, 0.0f));

        Cube cube2{};
        cube2.SetPosition(glm::vec3(0.0f, 100.0f, 0.0f));
        cube2.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));

        engine.AddObject(std::move(cube));
        engine.AddObject(std::move(cube2));

        // Render loop
        while (!window.ShouldClose()) {
            renderer.Clear();

            window.PickingPhase(engine, renderer);
            engine.ProcessInputs(window);
            engine.UserAction(window);
            engine.RenderObjects(renderer);
            // Draw a simple triangle
            // glClear(GL_COLOR_BUFFER_BIT);
            // glBegin(GL_TRIANGLES);
            // glVertex2f(-0.5f, -0.5f);
            // glVertex2f(0.5f, -0.5f);
            // glVertex2f(0.0f, 0.5f);
            // glEnd();
            // glfwSwapBuffers(window.window);

            window.ImGuiRender();
            window.SwapBuffersAndPollEvents();
            continue;
            float positions[6] = {
                -0.5f, -0.5f,
                0.5f, -0.5f,
                0.0f, 0.5f,
            };

            unsigned int buffer;
            GLCall(glGenBuffers(1, &buffer)); // create a vertex buffer
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); // bind
            GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, positions, GL_STATIC_DRAW)); // fill

            GLCall(glEnableVertexAttribArray(0));
            GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

            Shader myShader = Shader(vertexShaderPath, fragmentShaderPath);
            myShader.Bind();
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

            /* Loop until the user closes the window */
            while (!glfwWindowShouldClose(window.window))
            {
                /* Render here */
                GLCall(glClear(GL_COLOR_BUFFER_BIT));

                GLCall(glDrawArrays(GL_TRIANGLES, 0, 3));

                /* Swap front and back buffers */
                glfwSwapBuffers(window.window);

                /* Poll for and process events */
                glfwPollEvents();
            }

        }
    }

    return 0;
}
