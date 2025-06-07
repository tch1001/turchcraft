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


int main() {
    const std::string vertexShaderPath = std::format("{}/res/shaders/Vertex.shader", PROJECT_SOURCE_DIR);
    const std::string fragmentShaderPath = std::format("{}/res/shaders/Fragment.shader", PROJECT_SOURCE_DIR);
    const std::string pickingFragmentShaderPath = std::format("{}/res/shaders/PickingFragment.shader", PROJECT_SOURCE_DIR);
    Window window{WINDOW_WIDTH, WINDOW_HEIGHT, "TurchCraft"};
    // Create a framebuffer for mouse selection
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // generate picking texture
    GLuint textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    GLuint depthTexture;
    GLCall(glGenTextures(1, &depthTexture));
    GLCall(glBindTexture(GL_TEXTURE_2D, depthTexture));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
        exit(1);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    window.InitPicking(vertexShaderPath, pickingFragmentShaderPath);
    window.framebuffer_ = framebuffer;
    window.textureColorBuffer_ = textureColorBuffer;
    window.depthTexture_ = depthTexture;
    window.InitImGui();
    window.InitOtherStuff();

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
        engine.AddObject(std::move(cube));

        Cube cube2{};
        cube2.SetPosition(glm::vec3(0.0f, 100.0f, 0.0f));
        cube2.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
        engine.AddObject(std::move(cube2));

        // Render loop
        while (!window.ShouldClose()) {
            renderer.Clear();
            auto [pick, cubeSelected, faceSelected] = window.PickingPhase(engine, renderer);
            engine.ProcessInputs(window);
            int rightClick = engine.UserAction(window);
            std::cout << rightClick << " " << faceSelected << std::endl;
            engine.RenderObjects(renderer);
            if (rightClick != 0 && pick == 1) {
                for (const Cube& c : engine.objects) {
                    if (c.GetId() == cubeSelected) {
                        if (rightClick == 1) {
                            Cube newCube{};
                            glm::vec3 newPosition = c.position;
                            switch (faceSelected) {
                                case 4: newPosition += glm::vec3(100.0f, 0.f, 0.0);
                                    break;
                                case 3: newPosition += glm::vec3(-100.0f, 0.f, 0.0);
                                    break;
                                case 1: newPosition += glm::vec3(0.0f, 0.0f, 100.f);
                                    break;
                                case 2: newPosition += glm::vec3(0.0f, 0.0f, -100.f);
                                    break;
                                case 5: newPosition += glm::vec3(0.0f, 100.f, 0.0);
                                    break;
                                case 6: newPosition += glm::vec3(0.0f, -100.f, 0.0);
                                    break;
                            }
                            newCube.SetPosition(newPosition);
                            newCube.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
                            engine.AddObject(std::move(newCube));
                        } else {
                            std::erase(engine.objects, c);
                            break;
                        }
                    }
                }
            }

            window.ImGuiRender();
            window.SwapBuffersAndPollEvents();
        }
    }

    return 0;
}
