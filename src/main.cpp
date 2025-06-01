#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <format>
#include <chrono>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle", NULL, NULL);
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

    // Set the viewport
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    std::cout << "GLFW Version: " << glfwGetVersionString() << std::endl;
    std::cout << "GLAD Version: " << GLVersion.major << "." << GLVersion.minor << std::endl;

    {
        float positions[] = {
            // Front face
            -50.0f, -50.0f,  50.0f, 0.0f, 0.0f,
             50.0f, -50.0f,  50.0f, 1.0f, 0.0f,
             50.0f,  50.0f,  50.0f, 1.0f, 1.0f,
            -50.0f,  50.0f,  50.0f, 0.0f, 1.0f,

            // Back face
            -50.0f, -50.0f, -50.0f, 0.0f, 0.0f,
             50.0f, -50.0f, -50.0f, 1.0f, 0.0f,
             50.0f,  50.0f, -50.0f, 1.0f, 1.0f,
            -50.0f,  50.0f, -50.0f, 0.0f, 1.0f,
        };

        unsigned int indices[] = {
            // Front face
            0, 1, 2,
            2, 3, 0,

            // Back face
            4, 5, 6,
            6, 7, 4,

            // Left face
            4, 0, 3,
            3, 7, 4,

            // Right face
            1, 5, 6,
            6, 2, 1,

            // Top face
            3, 2, 6,
            6, 7, 3,

            // Bottom face
            4, 5, 1,
            1, 0, 4
        };

        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_BLEND));

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexArray va;
        VertexBuffer vb(positions, sizeof(positions));
        VertexBufferLayout layout;
        layout.Push<float>(3); // position
        layout.Push<float>(2); // texture
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 36);

        const std::string vertexShaderPath = std::format("{}/res/shaders/Vertex.shader", PROJECT_SOURCE_DIR);
        const std::string fragmentShaderPath = std::format("{}/res/shaders/Fragment.shader", PROJECT_SOURCE_DIR);
        Shader shader(vertexShaderPath, fragmentShaderPath);
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        Texture texture(std::format("{}/res/textures/texture.png", PROJECT_SOURCE_DIR));
        shader.Bind();
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        float r = 0.0f;
        float increment = 0.01f;
        double lastX = 400, lastY = 300; // Initial mouse position
        bool firstMouse = true;
        float yaw = -90.0f, pitch = 0.0f; // Initial orientation

        auto startTime = std::chrono::high_resolution_clock::now();

        const glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 10000.0f);

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
        glm::mat4 modelB = glm::translate(glm::mat4(1.0f), glm::vec3(400.0f, 300.0f, 0));
        glm::mat4 mvp = proj * view * model;

        // Render loop
        while (!glfwWindowShouldClose(window)) {

            glfwPollEvents(); // Process events

            // Check if any keys are pressed
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }

            // Mouse dragging for rotation
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                double xpos, ypos;
                glfwGetCursorPos(window, &xpos, &ypos);

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

            renderer.Clear();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float>(currentTime - startTime).count();

            model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f));
            modelB = glm::rotate(glm::mat4(1.0f), time, glm::vec3(1.0f, 0.0f, 0.0f));

            mvp = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(va, ib, shader);

            mvp = proj * view * modelB;
            shader.SetUniformMat4f("u_MVP", mvp);
            // renderer.Draw(va, ib, shader);

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
