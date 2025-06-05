#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <format>
#include <chrono>

#include "Cube.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

static std::string ParseShader(const std::string& path) {
    std::ifstream stream(path);
    std::string line;
    std::stringstream ss;
    while (getline(stream, line)) {
        ss << line << "\n";
    }
    return ss.str();
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length);
        glGetShaderInfoLog(id, length, &length, message.data());
        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cerr << message.data() << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Triangle", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Load OpenGL functions using glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set the viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    std::cout << "GLFW Version: " << glfwGetVersionString() << std::endl;
    std::cout << "GLAD Version: " << GLVersion.major << "." << GLVersion.minor << std::endl;

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

    {
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_BLEND));

        Cube cube{}, cube2{};
        cube.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        cube.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));

        cube2.SetPosition(glm::vec3(0.0f, 100.0f, 0.0f));
        cube2.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
        const std::string vertexShaderPath = std::format("{}/res/shaders/Vertex.shader", PROJECT_SOURCE_DIR);
        const std::string fragmentShaderPath = std::format("{}/res/shaders/Fragment.shader", PROJECT_SOURCE_DIR);
        const std::string pickingFragmentShaderPath = std::format("{}/res/shaders/PickingFragment.shader", PROJECT_SOURCE_DIR);
        Shader shader(vertexShaderPath, fragmentShaderPath);
        Shader pickingShader(vertexShaderPath, pickingFragmentShaderPath);

        Texture texture(std::format("{}/res/textures/texture.png", PROJECT_SOURCE_DIR));
        shader.Bind();
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        Renderer renderer;

        float r = 0.0f;
        float increment = 0.01f;
        double lastX = WINDOW_WIDTH / 2.0, lastY = WINDOW_HEIGHT / 2.0; // Initial mouse position
        bool firstMouse = true;
        float yaw = -90.0f, pitch = 0.0f; // Initial orientation

        auto startTime = std::chrono::high_resolution_clock::now();

        const glm::mat4 proj = glm::perspective(glm::radians(45.0f), static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 10000.0f);

        const float radius = 500.0f; // Radius of the sphere
        const glm::vec3 home_eye = glm::vec3(0.0f, 0.0f, 500.0f);
        const glm::vec3 home_front = glm::vec3(0.0f, 0.0f, -1.0f);
        const glm::vec3 home_center = home_eye + home_front * radius;
        const glm::vec3 home_up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::vec3 eye = home_eye;
        glm::vec3 current_center = home_center;
        glm::vec3 front = home_front;
        glm::vec3 up = home_up;

        glm::mat4 view = glm::lookAt(eye, current_center, up);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::mat4 modelB = glm::translate(glm::mat4(1.0f), glm::vec3(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0));
        glm::mat4 mvp = proj * view * model;

        // Render loop
        while (!glfwWindowShouldClose(window)) {

            glfwPollEvents(); // Process events

            // Check if any keys are pressed
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }

            // Bind the framebuffer for mouse selection
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Render the scene to the framebuffer
            pickingShader.Bind();
            pickingShader.SetUniform1ui("u_Id", cube.GetId());
            renderer.Draw(cube, pickingShader, proj, view);

            pickingShader.Bind();
            pickingShader.SetUniform1ui("u_Id", cube2.GetId());
            renderer.Draw(cube2, pickingShader, proj, view);

            // Read the pixel under the mouse cursor
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            std::cout << xpos << ", " << ypos << " ";
            unsigned int pixelData[3];
            glReadPixels(static_cast<int>(xpos), WINDOW_HEIGHT - static_cast<int>(ypos), 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &pixelData);
            std::cout << "Pixel data: " << pixelData[0] << ", " << pixelData[1] << ", " << pixelData[2] << std::endl;

            // Unbind the framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Mouse dragging for rotation
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                if (firstMouse) {
                    lastX = xpos;
                    lastY = ypos;
                    firstMouse = false;
                }

                float xoffset = xpos - lastX;
                float yoffset = ypos - lastY; // Corrected: y-coordinates should not be reversed
                lastX = xpos;
                lastY = ypos;

                float sensitivity = 0.1f;
                xoffset *= sensitivity;
                yoffset *= sensitivity;

                yaw += xoffset;
                pitch -= yoffset; // Corrected: subtract yoffset to match typical camera pitch control

                if (pitch > 89.0f)
                    pitch = 89.0f;
                if (pitch < -89.0f)
                    pitch = -89.0f;

                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                
            } else {
                firstMouse = true; // Reset firstMouse when not dragging
            }
            float movement_speed = 1.0f;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                eye += front * movement_speed;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                eye -= front * movement_speed;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                eye -= glm::normalize(glm::cross(front, up)) * movement_speed;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                eye += glm::normalize(glm::cross(front, up)) * movement_speed;
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                eye += up * movement_speed;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                eye -= up * movement_speed;
            }
            if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
                eye = home_eye;
                current_center = home_center;
                front = home_front;
                up = home_up;
                yaw = -90.0f;
                pitch = 0.0f;
            }
            current_center = eye + radius * glm::normalize(front); // Keep current_center on the sphere
            view = glm::lookAt(eye, current_center, up);

            // Clear the color and depth buffer bits
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderer.Clear();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float>(currentTime - startTime).count();

            renderer.Draw(cube, shader, proj, view);
            renderer.Draw(cube2, shader, proj, view);

            if (r > 1.0f) {
                increment = -0.01f;
            } else if (r < 0.0f) {
                increment = 0.01f;
            }
            r += increment;
            
            ImGui::Begin("Hello, world!");
            ImGui::Text("Hello, world!");
            ImGui::End();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
