#include "tp_camera.h"

namespace Magma {

    ThirdPersonImpl::ThirdPersonImpl(WindowInput& input) : CameraImpl{ input } {
        updateViewMatrix();
        updatePerspectiveMatrix();
        m_Input.getWindowImpl().setMouseHidden(false);
        m_Input.getEventDispatcher().subscribe<WindowResizeEvent>(this);
        m_Input.getEventDispatcher().subscribe<MouseMoveEvent>(this);
        m_Input.getEventDispatcher().subscribe<MouseScrollEvent>(this);
    }

    ThirdPersonImpl::~ThirdPersonImpl() {
        m_Input.getEventDispatcher().unsubscribe<WindowResizeEvent>(this);
        m_Input.getEventDispatcher().unsubscribe<MouseMoveEvent>(this);
        m_Input.getEventDispatcher().unsubscribe<MouseScrollEvent>(this);
    }

    void ThirdPersonImpl::updateViewMatrix() {
        const glm::vec3 eye{
            sin(m_Rotation.x) * m_ViewRadius * cos(m_Rotation.y),
            sin(m_Rotation.y) * m_ViewRadius,
            cos(m_Rotation.x) * m_ViewRadius* cos(m_Rotation.y)};
        m_ViewMatrix = glm::lookAt(eye, m_Position, m_UpVector);
    }

    float ThirdPersonImpl::getZoomSpeed() const {
        float distance = m_ViewRadius * 0.2f;
        distance = std::max(distance, 0.1f);
        float speed = distance * distance;
        speed = std::min(std::max(speed, 1.0f), 25.0f); 
        return speed;
    }

    void ThirdPersonImpl::onEvent(const WindowResizeEvent& _event) {
        updatePerspectiveMatrix();
    }

    void ThirdPersonImpl::onEvent(const MouseMoveEvent& _event) {
        if (m_Input.isMouseBtnPressed(MouseButton::RIGHT)) {
            bool updateView = false;
            constexpr float mouseSensitivity = 0.005f;
            if (m_LastMousePosition.x >= 0.0) {
                const double dx = _event.m_X - m_LastMousePosition.x;
                m_Rotation.x += static_cast<float>(-dx) * mouseSensitivity;
                updateView = true;
            }
            if (m_LastMousePosition.y >= 0.0) {
                const double dy = _event.m_Y - m_LastMousePosition.y;
                m_Rotation.y = std::max(glm::radians(-89.9f), std::min(glm::radians(89.9f),
                    m_Rotation.y + static_cast<float>(dy) * mouseSensitivity));
                updateView = true; 
            }
            if (updateView) updateViewMatrix();
        }
        m_LastMousePosition = { _event.m_X, _event.m_Y };
    }

    void ThirdPersonImpl::onEvent(const MouseScrollEvent& _event) {
        m_ViewRadius = std::max(0.1f, m_ViewRadius + static_cast<float>(-_event.m_Y) * 0.5f * getZoomSpeed());
        updateViewMatrix();
    }

}