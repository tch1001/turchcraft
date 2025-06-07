#include "Renderer.h"

#include "Cube.h"
#include "glm/ext/matrix_transform.hpp"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::Clear() const {
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
    shader.Bind();
    va.Bind();
    ib.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

class Cube;

void Renderer::Draw(const Cube& c, Shader& shader, const glm::mat4& proj, const glm::mat4& view) {
    glm::mat4 model = c.GetModelMatrix();

    glm::mat4 mvp = proj * view * model;
    shader.Bind();
    shader.SetUniformMat4f("u_MVP", mvp);
    c.Draw();
}
