//
// Created by tanch on 6/6/2025.
//

#include "Engine.h"

#include "Window.h"
#include "GLFW/glfw3.h"
#include "glm/ext/matrix_transform.hpp"
#include "Cube.h"

Engine::Engine(const Config &config)
    : graphicsShader{config.vertexShaderPath, config.graphicsFragmentShaderPath}
    , windowWidth(config.windowWidth)
    , windowHeight(config.windowHeight)
    , lastX(windowWidth/2.0)
    , lastY(windowHeight/2.0)
    , proj(glm::template perspective<float>(glm::radians(45.0), static_cast<float>(windowWidth) / windowHeight, 0.1f, 10000.0f))
{}

Engine::~Engine() {}

int Engine::UserAction(Window &window) {
    if (window.GetMouseRightButton() == GLFW_PRESS) {
        if (firstRightMouse) {
            firstRightMouse = false;
            return 1;
        }
    } else {
        firstRightMouse = true;
    }
    bool leftClick = false;
    // Mouse dragging for rotation
    if (window.GetMouseButton() == GLFW_PRESS) {

        auto [xPos, yPos] = window.GetCursorPos();
        if (firstMouse) {
            lastX = xPos;
            lastY = yPos;
            firstMouse = false;
            leftClick = true;
        }

        float xoffset = xPos - lastX;
        float yoffset = yPos - lastY; // Corrected: y-coordinates should not be reversed
        lastX = xPos;
        lastY = yPos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch -= yoffset; // Corrected: subtract yoffset to match typical camera pitch control

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch<-89.0f)
            pitch = -89.0f;

        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    } else {
        firstMouse = true; // Reset firstMouse when not dragging
    }
    float movement_speed = 1.0f;
    if (window.GetKey(GLFW_KEY_W) == GLFW_PRESS) {
        eye += front * movement_speed;
    }
    if (window.GetKey(GLFW_KEY_S) == GLFW_PRESS) {
        eye -= front * movement_speed;
    }
    if (window.GetKey(GLFW_KEY_A) == GLFW_PRESS) {
        eye -= glm::normalize(glm::cross(front, up)) * movement_speed;
    }
    if (window.GetKey(GLFW_KEY_D) == GLFW_PRESS) {
        eye += glm::normalize(glm::cross(front, up)) * movement_speed;
    }
    if (window.GetKey(GLFW_KEY_SPACE) == GLFW_PRESS) {
        eye += up * movement_speed;
    }
    if (window.GetKey(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        eye -= up * movement_speed;
    }
    if (window.GetKey(GLFW_KEY_H) == GLFW_PRESS) {
        eye = home_eye;
        current_center = home_center;
        front = home_front;
        up = home_up;
        yaw = -90.0f;
        pitch = 0.0f;
    }
    current_center = eye + radius * glm::normalize(front); // Keep current_center on the sphere
    return leftClick ? 2 : 0;
}

void Engine::ProcessInputs(Window& window) {
    // Check if any keys are pressed
    if (window.GetKey(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        window.SetWindowShouldClose(true);
    }
}

glm::mat4 Engine::GetViewMatrix() {
    glm::mat4 view = glm::lookAt(eye, current_center, up);
    return view;
}

void Engine::RenderObjects(Renderer& renderer) {
    // graphicsShader.SetUniform4f("u_Color", 0.1f, 0.3f, 0.8f, 1.0f);

    for (auto const& obj : objects) {
        renderer.Draw(obj, graphicsShader, proj, GetViewMatrix());
    }
}

void Engine::AddObject(Cube &&cube) {
    objects.push_back(std::move(cube));
}
