#include "tp_camera.h"

namespace Magma {

    ThirdPersonImpl::ThirdPersonImpl(WindowInput& input) : input{ input } {
        updateViewMat4f();
        updatePerspectiveMat4f();

        input.getWindowImpl().setMouseHidden(false);

        input.getEventDispatcher().subscribe<WindowResizeEvent>(this);
        input.getEventDispatcher().subscribe<MouseMoveEvent>(this);
        input.getEventDispatcher().subscribe<MouseScrollEvent>(this);
    }

    ThirdPersonImpl::~ThirdPersonImpl() {
        input.getEventDispatcher().unsubscribe<WindowResizeEvent>(this);
        input.getEventDispatcher().unsubscribe<MouseMoveEvent>(this);
        input.getEventDispatcher().unsubscribe<MouseScrollEvent>(this);
    }

    void ThirdPersonImpl::updateViewMat4f() {
        glm::vec3 eye{
            sin(rotation.x) * radius * cos(rotation.y),
            sin(rotation.y) * radius,
            cos(rotation.x) * radius * cos(rotation.y)};
        viewMat4f = glm::lookAt(eye, position, upVec3f);
    }

    void ThirdPersonImpl::updatePerspectiveMat4f() {
        auto windowSize = input.getWindowImpl().getSize();
        perspectiveMat4f = glm::perspective(glm::radians(fov),
            (float)windowSize.first / (float)windowSize.second, zNear, zFar);
    }

    const glm::mat4& ThirdPersonImpl::getViewMat4f() const {
        return viewMat4f;
    }

    const glm::mat4& ThirdPersonImpl::getPerspectiveMat4f() const {
        return perspectiveMat4f;
    }

    float ThirdPersonImpl::getZoomSpeed() {
        float distance = radius * 0.2f;
        distance = std::max(distance, 0.1f);
        float speed = distance * distance;
        speed = std::min(std::max(speed, 1.0f), 25.0f); 
        return speed;
    }

    void ThirdPersonImpl::onEvent(const WindowResizeEvent& _event) {
        updatePerspectiveMat4f();
    }

    void ThirdPersonImpl::onEvent(const MouseMoveEvent& _event) {
        if (input.isMouseBtnPressed(MouseButton::RIGHT)) {
            bool updateView = false;
            const float sensitivity = 0.005f;
            if (lastMousePos.x >= 0.0f) {
                float dx = _event.x - lastMousePos.x;
                rotation.x += -dx * sensitivity;
                updateView = true;
            }
            if (lastMousePos.y >= 0.0f) {
                float dy = _event.y - lastMousePos.y;
                rotation.y = std::max(glm::radians(-89.9f), std::min(glm::radians(89.9f), 
                    rotation.y + dy * sensitivity));
                updateView = true; 
            }
            if (updateView) updateViewMat4f();
        }
        lastMousePos = { _event.x, _event.y };
    }

    void ThirdPersonImpl::onEvent(const MouseScrollEvent& _event) {
        radius = std::max(0.1f, radius + (float)(-_event.y) * 0.5f * getZoomSpeed());
        updateViewMat4f();
    }

}