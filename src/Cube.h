#pragma once
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "glm/glm.hpp"


    const float positions[] = {
            // Front face               // Texture coordinates      // vertex Color
            -50.0f, -50.0f,  50.0f,     0.0f, 0.0f,                 0.3f, 0.7f, 0.2f,
             50.0f, -50.0f,  50.0f,     1.0f, 0.0f,                 0.6f, 0.1f, 0.8f,
             50.0f,  50.0f,  50.0f,     1.0f, 1.0f,                 0.9f, 0.4f, 0.3f,
            -50.0f,  50.0f,  50.0f,     0.0f, 1.0f,                 0.2f, 0.5f, 0.9f,

            // Back face
            -50.0f, -50.0f, -50.0f,     0.0f, 0.0f,                 0.7f, 0.3f, 0.1f,
             50.0f, -50.0f, -50.0f,     1.0f, 0.0f,                 0.4f, 0.8f, 0.6f,
             50.0f,  50.0f, -50.0f,     1.0f, 1.0f,                 0.5f, 0.2f, 0.7f,
            -50.0f,  50.0f, -50.0f,     0.0f, 1.0f,                 0.8f, 0.6f, 0.4f,

            // Left face
            -50.0f, -50.0f, -50.0f,     0.0f, 0.0f,                 0.1f, 0.9f, 0.5f,
            -50.0f, -50.0f,  50.0f,     1.0f, 0.0f,                 0.3f, 0.2f, 0.8f,
            -50.0f,  50.0f,  50.0f,     1.0f, 1.0f,                 0.6f, 0.4f, 0.1f,
            -50.0f,  50.0f, -50.0f,     0.0f, 1.0f,                 0.7f, 0.5f, 0.3f,

            // Right face
             50.0f, -50.0f, -50.0f,     0.0f, 0.0f,                 0.2f, 0.6f, 0.9f,
             50.0f, -50.0f,  50.0f,     1.0f, 0.0f,                 0.5f, 0.3f, 0.7f,
             50.0f,  50.0f,  50.0f,     1.0f, 1.0f,                 0.8f, 0.1f, 0.4f,
             50.0f,  50.0f, -50.0f,     0.0f, 1.0f,                 0.9f, 0.7f, 0.2f,

            // Top face
            -50.0f,  50.0f,  50.0f,     0.0f, 0.0f,                 0.4f, 0.8f, 0.5f,
             50.0f,  50.0f,  50.0f,     1.0f, 0.0f,                 0.1f, 0.6f, 0.3f,
             50.0f,  50.0f, -50.0f,     1.0f, 1.0f,                 0.7f, 0.2f, 0.9f,
            -50.0f,  50.0f, -50.0f,     0.0f, 1.0f,                 0.3f, 0.5f, 0.1f,

            // Bottom face
            -50.0f, -50.0f,  50.0f,     0.0f, 0.0f,                 0.6f, 0.4f, 0.7f,
             50.0f, -50.0f,  50.0f,     1.0f, 0.0f,                 0.2f, 0.9f, 0.5f,
             50.0f, -50.0f, -50.0f,     1.0f, 1.0f,                 0.8f, 0.3f, 0.6f,
            -50.0f, -50.0f, -50.0f,     0.0f, 1.0f,                 0.5f, 0.1f, 0.4f,
        };

    const unsigned int indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
            4, 5, 6,
            6, 7, 4,

            // Left face
            8, 9, 10,
            10, 11, 8,

            // Right face
            12, 13, 14,
            14, 15, 12,

            // Top face
            16, 17, 18,
            18, 19, 16,

            // Bottom face
            20, 21, 22,
            22, 23, 20
        };

class Cube {
public:
    Cube();
    ~Cube();
    [[nodiscard]] glm::uint GetId() const;
    void Draw() const;
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    [[nodiscard]] glm::mat4 GetModelMatrix() const;
private:
    VertexArray m_VA;
    VertexBuffer m_VBO;
    IndexBuffer m_IBO;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::uint id;
};

