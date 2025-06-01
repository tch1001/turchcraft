#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <format>

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
             -50.0f, -50.0f,    0.0f, 0.0f,
              50.0f, -50.0f,    1.0f, 0.0f,
              50.0f,  50.0f,    1.0f, 1.0f,
             -50.0f,  50.0f,    0.0f, 1.0f,
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_BLEND));

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexArray va;
        VertexBuffer vb(positions, sizeof(positions));
        VertexBufferLayout layout;
        layout.Push<float>(2); // position
        layout.Push<float>(2); // texture
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);

        const std::string vertexShaderPath = std::format("{}/res/shaders/Vertex.shader", PROJECT_SOURCE_DIR);
        const std::string fragmentShaderPath = std::format("{}/res/shaders/Fragment.shader", PROJECT_SOURCE_DIR);
        Shader shader(vertexShaderPath, fragmentShaderPath);
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        glm::mat4 proj = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::mat4 modelB = glm::translate(glm::mat4(1.0f), glm::vec3(400.0f, 300.0f, 0));
        glm::mat4 mvp = proj * view * model;

        shader.SetUniformMat4f("u_MVP", mvp);

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
        // Render loop
        while (!glfwWindowShouldClose(window)) {
            renderer.Clear();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            mvp = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(va, ib, shader);

            mvp = proj * view * modelB;
            shader.SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(va, ib, shader);

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
