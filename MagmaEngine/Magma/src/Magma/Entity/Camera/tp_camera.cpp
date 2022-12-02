#include "tp_camera.h"

namespace Magma {

    ThirdPersonImpl::ThirdPersonImpl(WindowImpl& windowImpl) {
        // TODO: add event system instead of this
        windowImpl.addMouseButtonCallback([&](MouseButton button, bool pressed, int mods) {
            if (button == MouseButton::RIGHT) {
                rightMouseBtnPressed = pressed;
            }
        });

        windowImpl.addMousePosCallback([&](double x, double y) {
            const float sensitivity = 0.005f;
            if (rightMouseBtnPressed) {
                if (lastMousePos.x >= 0.0f && lastMousePos.y >= 0.0f) {
                    float dx = x - lastMousePos.x;
                    float dy = y - lastMousePos.y;
                    rotation[0] += -dx * sensitivity;
                    rotation[1] = std::max(glm::radians(-89.9f),
                        std::min(glm::radians(89.9f), rotation[1] + dy * sensitivity));
                }
            }
            lastMousePos = { x, y };
        });

        windowImpl.addMouseScrollCallback([&](double x, double y) {
            radius = std::max(0.1f, radius + (float)(-y) * 0.5f * getZoomSpeed());
        });
    }

    glm::mat4 ThirdPersonImpl::getViewMatrix() {
        glm::vec3 eye{
            sin(rotation[0]) * radius * cos(rotation[1]),
            sin(rotation[1]) * radius,
            cos(rotation[0]) * radius * cos(rotation[1])
        };

        return glm::lookAt(eye, position, upVector);
    }

    glm::mat4 ThirdPersonImpl::getPerspectiveMatrix() {
        return glm::mat4{ 1.0f };
    }

    float ThirdPersonImpl::getZoomSpeed() {
        // this code is from the Hazel Engine
        float distance = radius * 0.2f;
        distance = std::max(distance, 0.1f);
        float speed = distance * distance;
        speed = std::min(std::max(speed, 1.0f), 25.0f); 
        return speed;
    }

}