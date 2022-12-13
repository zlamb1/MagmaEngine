#include "fp_camera.h"

namespace Magma {

	FirstPersonImpl::FirstPersonImpl(WindowInput& input) : CameraImpl{input} {
		input.getWindowImpl().setMouseCentered();
		// set default camera position
		m_Position = { 0.0f, 0.0f, -3.0f };

		// call to virtual function in constructor
		// possible issue
		updateViewMatrix();
		updatePerspectiveMatrix();

		setAcceptInput(input.getWindowImpl().isFocused());

		// register listeners
		input.getEventDispatcher().subscribe<WindowResizeEvent>(this);
		input.getEventDispatcher().subscribe<WindowFocusEvent>(this);
		input.getEventDispatcher().subscribe<MouseButtonEvent>(this);
		input.getEventDispatcher().subscribe<MouseMoveEvent>(this);
	}

	FirstPersonImpl::~FirstPersonImpl() {
		m_Input.getEventDispatcher().unsubscribe<WindowResizeEvent>(this);
		m_Input.getEventDispatcher().unsubscribe<WindowFocusEvent>(this);
		m_Input.getEventDispatcher().unsubscribe<MouseButtonEvent>(this);
		m_Input.getEventDispatcher().unsubscribe<MouseMoveEvent>(this);
	}

	void FirstPersonImpl::onUpdate(Timestep step) {
		if (m_AcceptInput) {
			glm::vec3 velocity{ 0.0f };
			constexpr float movementSpeed = 0.2f;
			const auto forwardVec3f = getForwardVec3f(false);
			const auto rightVec3f = glm::cross(forwardVec3f, m_UpVector);

			if (m_Input.isKeyPressed(KeyButton::W))
				velocity += forwardVec3f;
			if (m_Input.isKeyPressed(KeyButton::S))
				velocity -= forwardVec3f; 
			if (m_Input.isKeyPressed(KeyButton::A))
				velocity += rightVec3f; 
			if (m_Input.isKeyPressed(KeyButton::D))
				velocity -= rightVec3f;
			if (m_Input.isKeyPressed(KeyButton::SPACE))
				velocity += -m_UpVector; 
			if (m_Input.isKeyPressed(KeyButton::LEFT_CONTROL))
				velocity -= -m_UpVector;

			m_Position += velocity * movementSpeed * static_cast<float>(m_TargetFps / step.getFps());
			if (velocity != glm::vec3{ 0.0f })
				updateViewMatrix();
		}
	}

	void FirstPersonImpl::updateViewMatrix() {
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + getForwardVec3f(true), m_UpVector);
	}

	glm::vec3 FirstPersonImpl::getForwardVec3f(bool includeY) const {
		if (includeY)
			return glm::vec3{
				sin(m_Rotation.x) * cos(m_Rotation.y),
				sin(m_Rotation.y),
				cos(m_Rotation.x) * cos(m_Rotation.y)
		};
		else
			return glm::vec3{ sin(m_Rotation.x), 0.0f, cos(m_Rotation.x) };
	}

	void FirstPersonImpl::setAcceptInput(const bool acceptInput) {
		if (acceptInput) {
			m_Input.getWindowImpl().setMouseCentered();
		}
		m_Input.getWindowImpl().setMouseHidden(acceptInput);
		this->m_AcceptInput = acceptInput;
	}

	void FirstPersonImpl::onEvent(const WindowResizeEvent& _event) {
		updatePerspectiveMatrix();
	}

	void FirstPersonImpl::onEvent(const WindowFocusEvent& _event) {
		if (!_event.m_Focused) {
			setAcceptInput(false);
		}
	}

	void FirstPersonImpl::onEvent(const MouseButtonEvent& _event) {
		const auto [x, y] = m_Input.getWindowImpl().getMousePosition();
		if (_event.m_Button == MouseButton::LEFT && _event.m_Pressed && y >= 0) {
			setAcceptInput(true);
		}
	}

	void FirstPersonImpl::onEvent(const MouseMoveEvent& _event) {
		if (m_AcceptInput) {
			constexpr float mouseSensitivity = 0.002f;

			auto [width, height] = m_Input.getWindowImpl().getSize();
			const float dx = static_cast<float>(_event.m_X) - static_cast<float>(width) / 2.0f;
			const float dy = static_cast<float>(_event.m_Y) - static_cast<float>(height) / 2.0f;
			m_Rotation.x += -dx * mouseSensitivity;
			m_Rotation.y += -dy * mouseSensitivity;
			m_Rotation.y = glm::clamp(m_Rotation.y, glm::radians(-89.9f), glm::radians(89.9f));
			m_Input.getWindowImpl().setMouseCentered();
			updateViewMatrix();
		}
	}

}