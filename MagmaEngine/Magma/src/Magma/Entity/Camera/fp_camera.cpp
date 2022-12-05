#include "fp_camera.h"

namespace Magma {

	FirstPersonImpl::FirstPersonImpl(WindowInput& input) : input{ input } {
		input.getWindowImpl().setMouseCentered();
		position = { 0.0f, 0.0f, -3.0f };
		updateViewMat4f();
		updatePerspectiveMat4f();

		setAcceptInput(input.getWindowImpl().isFocused());

		input.getEventDispatcher().subscribe<WindowResizeEvent>(this);
		input.getEventDispatcher().subscribe<WindowFocusEvent>(this);
		input.getEventDispatcher().subscribe<MouseButtonEvent>(this);
		input.getEventDispatcher().subscribe<MouseMoveEvent>(this);
	}

	FirstPersonImpl::~FirstPersonImpl() {
		input.getEventDispatcher().unsubscribe<WindowResizeEvent>(this);
		input.getEventDispatcher().unsubscribe<WindowFocusEvent>(this);
		input.getEventDispatcher().unsubscribe<MouseButtonEvent>(this);
		input.getEventDispatcher().unsubscribe<MouseMoveEvent>(this);
	}

	void FirstPersonImpl::onUpdate(Timestep step) {
		if (acceptInput) {
			glm::vec3 velocity{ 0.0f };
			const float speed = 0.2f;
			auto forwardVec3f = getForwardVec3f(false);
			auto rightVec3f = glm::cross(forwardVec3f, upVec3f);

			if (input.isKeyPressed(KeyButton::W))
				velocity += forwardVec3f;
			if (input.isKeyPressed(KeyButton::S))
				velocity -= forwardVec3f; 
			if (input.isKeyPressed(KeyButton::A))
				velocity += rightVec3f; 
			if (input.isKeyPressed(KeyButton::D))
				velocity -= rightVec3f;
			if (input.isKeyPressed(KeyButton::SPACE))
				velocity += -upVec3f; 
			if (input.isKeyPressed(KeyButton::LEFT_CONTROL))
				velocity -= -upVec3f;

			position += velocity * speed * (float)(targetFps / step.getFps());
			if (velocity != glm::vec3{ 0.0f })
				updateViewMat4f();
		}
	}

	void FirstPersonImpl::updateViewMat4f() {
		viewMat4f = glm::lookAt(position, position + getForwardVec3f(true), upVec3f);
	}

	void FirstPersonImpl::updatePerspectiveMat4f() {
		auto windowSize = input.getWindowImpl().getSize();
		perspectiveMat4f = glm::perspective(glm::radians(fov),
			(float)windowSize.first / (float)windowSize.second, zNear, zFar);
	}

	const glm::mat4& FirstPersonImpl::getViewMat4f() const {
		return viewMat4f;
	}

	const glm::mat4& FirstPersonImpl::getPerspectiveMat4f() const {
		return perspectiveMat4f;
	}

	glm::vec3 FirstPersonImpl::getForwardVec3f(bool includeY) {
		if (includeY)
			return glm::vec3{
				sin(rotation.x) * cos(rotation.y),
				sin(rotation.y),
				cos(rotation.x) * cos(rotation.y)
		};
		else
			return glm::vec3{ sin(rotation.x), 0.0f, cos(rotation.x) };
	}

	void FirstPersonImpl::setAcceptInput(bool acceptInput) {
		if (acceptInput)
			input.getWindowImpl().setMouseCentered();
		input.getWindowImpl().setMouseHidden(acceptInput);
		this->acceptInput = acceptInput;
	}

	void FirstPersonImpl::onEvent(const WindowResizeEvent& _event) {
		updatePerspectiveMat4f();
	}

	void FirstPersonImpl::onEvent(const WindowFocusEvent& _event) {
		if (!_event.focused) {
			setAcceptInput(false);
		}
	}

	void FirstPersonImpl::onEvent(const MouseButtonEvent& _event) {
		auto mousePosition = input.getWindowImpl().getMousePosition();
		if (_event.btn == MouseButton::LEFT && _event.pressed && mousePosition.second >= 0) {
			setAcceptInput(true);
		}
	}

	void FirstPersonImpl::onEvent(const MouseMoveEvent& _event) {
		if (acceptInput) {
			auto windowSize = input.getWindowImpl().getSize();
			float dx = _event.x - windowSize.first / 2.0f;
			float dy = _event.y - windowSize.second / 2.0f;
			float sensitivity = 0.002f;
			rotation.x += -dx * sensitivity;
			rotation.y += -dy * sensitivity;
			rotation.y = glm::clamp(rotation.y, glm::radians(-89.9f), glm::radians(89.9f));
			input.getWindowImpl().setMouseCentered();
			updateViewMat4f();
		}
	}

}