#pragma once
#include "Engine.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "glm/glm.hpp"


    const float positions[] = {
            // -0.5f, -0.5f,
            // 0.5f, -0.5f,
            // 0.5f, 0.5f,
            // -0.5f, 0.5f,
            // Front face               // Texture coordinates      // Vertex Color     // face id
            -50.0f, -50.0f,  50.0f,     0.0f, 0.0f,                 0.3f, 0.7f, 0.2f,   1.0f,
             50.0f, -50.0f,  50.0f,     1.0f, 0.0f,                 0.6f, 0.1f, 0.8f, 1.0f,
             50.0f,  50.0f,  50.0f,     1.0f, 1.0f,                 0.9f, 0.4f, 0.3f, 1.0f,
            -50.0f,  50.0f,  50.0f,     0.0f, 1.0f,                 0.2f, 0.5f, 0.9f, 1.0f,

            // Back face
            -50.0f, -50.0f, -50.0f,     0.0f, 0.0f,                 0.7f, 0.3f, 0.1f, 2.0f,
             50.0f, -50.0f, -50.0f,     1.0f, 0.0f,                 0.4f, 0.8f, 0.6f, 2.0f,
             50.0f,  50.0f, -50.0f,     1.0f, 1.0f,                 0.5f, 0.2f, 0.7f, 2.0f,
            -50.0f,  50.0f, -50.0f,     0.0f, 1.0f,                 0.8f, 0.6f, 0.4f, 2.0f,

            // Left face
            -50.0f, -50.0f, -50.0f,     0.0f, 0.0f,                 0.1f, 0.9f, 0.5f, 3.0f,
            -50.0f, -50.0f,  50.0f,     1.0f, 0.0f,                 0.3f, 0.2f, 0.8f, 3.0f,
            -50.0f,  50.0f,  50.0f,     1.0f, 1.0f,                 0.6f, 0.4f, 0.1f, 3.0f,
            -50.0f,  50.0f, -50.0f,     0.0f, 1.0f,                 0.7f, 0.5f, 0.3f, 3.0f,

            // Right face
             50.0f, -50.0f, -50.0f,     0.0f, 0.0f,                 0.2f, 0.6f, 0.9f, 4.0f,
             50.0f, -50.0f,  50.0f,     1.0f, 0.0f,                 0.5f, 0.3f, 0.7f, 4.0f,
             50.0f,  50.0f,  50.0f,     1.0f, 1.0f,                 0.8f, 0.1f, 0.4f, 4.0f,
             50.0f,  50.0f, -50.0f,     0.0f, 1.0f,                 0.9f, 0.7f, 0.2f, 4.0f,

            // Top face
            -50.0f,  50.0f,  50.0f,     0.0f, 0.0f,                 0.4f, 0.8f, 0.5f, 5.0f,
             50.0f,  50.0f,  50.0f,     1.0f, 0.0f,                 0.1f, 0.6f, 0.3f, 5.0f,
             50.0f,  50.0f, -50.0f,     1.0f, 1.0f,                 0.7f, 0.2f, 0.9f, 5.0f,
            -50.0f,  50.0f, -50.0f,     0.0f, 1.0f,                 0.3f, 0.5f, 0.1f, 5.0f,

            // Bottom face
            -50.0f, -50.0f,  50.0f,     0.0f, 0.0f,                 0.6f, 0.4f, 0.7f, 6.0f,
             50.0f, -50.0f,  50.0f,     1.0f, 0.0f,                 0.2f, 0.9f, 0.5f, 6.0f,
             50.0f, -50.0f, -50.0f,     1.0f, 1.0f,                 0.8f, 0.3f, 0.6f, 6.0f,
            -50.0f, -50.0f, -50.0f,     0.0f, 1.0f,                 0.5f, 0.1f, 0.4f, 6.0f,
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
    Cube(const Cube&) = delete;
    Cube& operator=(const Cube&) = delete;

    void DrawForPicking(Shader &pickingShader, Engine &engine) const;

    Cube(Cube&&) = default;
    Cube& operator=(Cube&&) = default;
    ~Cube();
    [[nodiscard]] glm::uint GetId() const;
    void Draw() const;
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    [[nodiscard]] glm::mat4 GetModelMatrix() const;

    bool operator==(const Cube &rhs) const;

    // private:
    VertexArray m_VA;
    VertexBuffer m_VBO;
    IndexBuffer m_IBO;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::uint id;
};

