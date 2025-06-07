//
// Created by tanch on 6/6/2025.
//

#ifndef ENGINE_H
#define ENGINE_H
#include "Window.h"
#include "glm/ext/matrix_clip_space.hpp"

class Shader;

class Engine {
public:
    struct Config {
        std::string vertexShaderPath;
        std::string graphicsFragmentShaderPath;
        double windowWidth;
        double windowHeight;
    };
    explicit Engine(const Config& config);
    ~Engine();

    void UserAction(Window &window);

    glm::mat4 GetViewMatrix();
    void ProcessInputs(Window &window);

    void RenderObjects(Renderer &renderer);

    void AddObject(Cube &&cube);

    std::vector<Cube> objects;
    Shader graphicsShader;
private:
    bool firstMouse = true;
    double windowWidth, windowHeight;
    double lastX, lastY;

    const glm::vec3 home_eye = glm::vec3(0.0f, 0.0f, 500.0f);
    const glm::vec3 home_front = glm::vec3(0.0f, 0.0f, -1.0f);
    const float radius = 500.0f; // Radius of the sphere
    const glm::vec3 home_center = home_eye + home_front * radius;
    const glm::vec3 home_up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 eye = home_eye;
    glm::vec3 current_center = home_center;
    glm::vec3 front = home_front;
    glm::vec3 up = home_up;

    const glm::mat4 proj;

public:
    [[nodiscard]] glm::mat4 proj1() const {
        return proj;
    }

private:
    float yaw = -90.0f, pitch = 0.0f; // Initial orientation
};



#endif //ENGINE_H
