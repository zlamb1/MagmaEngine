#include "tp_camera.h"

using namespace Camera; 
using namespace Window;

ThirdPersonImpl::ThirdPersonImpl(WindowImpl& windowImpl) {
    windowImpl.addMouseButtonCallback([&](MouseButton button, bool pressed, int mods) {
        if (button == MouseButton::RIGHT) {
            rightMouseBtnPressed = pressed; 
        }
    });
    windowImpl.addMousePosCallback([&](double x, double y) {
        const float sensitivity = 0.005f;
        if (rightMouseBtnPressed) {
            if (lastX >= 0.0f && lastY >= 0.0f) {
                float dx = x - lastX;
                float dy = y - lastY;
                rotation[0] += -dx * sensitivity;
                rotation[1] = std::max(glm::radians(-89.9f),
                    std::min(glm::radians(89.9f), rotation[1] + dy * sensitivity));
            } 
        }
        lastX = x;
        lastY = y;
    });
    windowImpl.addMouseScrollCallback([&](double x, double y) {
        if (radius >= 1.0f) {
            radius = std::max(1.0, radius + -y * 0.5f);
        }
    });
}

glm::mat4 ThirdPersonImpl::getViewMatrix() {
    glm::vec3 eye{
        sin(rotation[0])* radius * cos(rotation[1]),
        sin(rotation[1]) * radius,
        cos(rotation[0]) * radius * cos(rotation[1])
    };

    return glm::lookAt(eye, position, upVector);
}