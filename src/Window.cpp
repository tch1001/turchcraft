//
// Created by tanch on 6/6/2025.
//

#include "Window.h"

#include <complex>
#include <iostream>
#include <stdexcept>

#include "Engine.h"
#include "Renderer.h"
#include "glad/glad.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

Window::Window(int width, int height, const char *title)
    : width_(width)
      , height_(height)
      , pickingShader()
{
    // Initialize GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwSetErrorCallback([](int error, const char *description) {
        std::cerr << "GLFW error: " <<  description << std::endl;
    });
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    this->window = window;
    glfwMakeContextCurrent(this->window);
    glfwSwapInterval(1);

    // Load OpenGL functions using glad
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }
}

Window::~Window() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void Window::InitImGui() {
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    GLCall(glViewport(0, 0, width, height));
}

void Window::InitOtherStuff() {
    // Enable depth testing
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));


    // Set the viewport
    GLCall(glViewport(0, 0, width_, height_));
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_BLEND));
}

void Window::InitPicking(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    pickingShader = Shader{vertexShaderPath, fragmentShaderPath};

    unsigned int framebuffer, textureColorBuffer, depthTexture;
    GLCall(glGenFramebuffers(1, &framebuffer));
    framebuffer_ = framebuffer;
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_));

    GLCall(glGenTextures(1, &textureColorBuffer));
    textureColorBuffer_ = textureColorBuffer;
    GLCall(glBindTexture(GL_TEXTURE_2D, textureColorBuffer_));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, width_, height_, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer_, 0));

    GLCall(glGenTextures(1, &depthTexture));
    depthTexture_ = depthTexture;
    GLCall(glBindTexture(GL_TEXTURE_2D, depthTexture_));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture_, 0));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
        exit(1);
    }

    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Window::PickingPhase(Engine& engine, Renderer& renderer) {
    // Bind the framebuffer for mouse selection
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    for (const Cube &obj : engine.objects) {
        pickingShader.Bind();
        pickingShader.SetUniform1ui("u_Id", obj.GetId());
        renderer.Draw(obj, pickingShader, engine.proj1(), engine.GetViewMatrix());
    }

    // Read the pixel under the mouse cursor
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    unsigned int pixelData[3];
    GLCall(glReadPixels(static_cast<int>(xpos), width_ - static_cast<int>(ypos), 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT,
                 &pixelData));
    std::cout << "Pixel data: " << pixelData[0] << ", " << pixelData[1] << ", " << pixelData[2] << std::endl;

    // Unbind the framebuffer
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    // Clear the color and depth buffer bits
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

bool Window::GetMouseButton() const {
    return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
}

void Window::ImGuiRender() {
    ImGui::Begin("Hello, world!");
    ImGui::Text("Hello, world!");
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::SwapBuffersAndPollEvents() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(window);
}

int Window::GetKey(int i) {
    return glfwGetKey(window, i);
}

std::pair<double, double> Window::GetCursorPos() const {
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    return std::make_pair<>(xPos, yPos);
}

void Window::SetWindowShouldClose(bool cond) {
    glfwSetWindowShouldClose(window, cond);
}
