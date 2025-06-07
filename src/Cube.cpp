#include "Cube.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "glm/gtc/matrix_transform.hpp"

static glm::uint CUBES = 2; // 0 represents draw normally, 1 represents background
Cube::Cube() 
    : m_VBO(positions, sizeof(positions))
    , m_IBO(indices, sizeof(indices) / sizeof(unsigned int)) 
    , id(CUBES++)
{
    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(2); // texture
    layout.Push<float>(3); // color
    layout.Push<float>(1); // face
    m_VA.AddBuffer(m_VBO, layout);
}

void Cube::DrawForPicking(Shader& pickingShader, Engine &engine) const {
    pickingShader.Bind();
    pickingShader.SetUniform1ui("u_Id", GetId());
    glm::mat4 model = GetModelMatrix();

    glm::mat4 mvp = engine.proj1() * engine.GetViewMatrix() * model;
    pickingShader.SetUniformMat4f("u_MVP", mvp);
    Draw();
}

Cube::~Cube() {}

glm::uint Cube::GetId() const {
    return id;
}

void Cube::Draw() const {
    m_VA.Bind();
    m_IBO.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, m_IBO.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Cube::SetPosition(const glm::vec3 &position) {
    this->position = position;
}

void Cube::SetRotation(const glm::vec3 &rotation) {
    this->rotation = rotation;
}

glm::mat4 Cube::GetModelMatrix() const {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    return model;
}

bool Cube::operator==(const Cube& rhs) const {
    return this->id == rhs.id;
}