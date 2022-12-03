#include "window_input.h"

namespace Magma {

	WindowInput::WindowInput(WindowImpl& windowImpl) : windowImpl{ windowImpl } {
		registerCallbacks();
	}

	void WindowInput::registerCallbacks() {
		windowImpl.addWindowSizeCallback([&](int32_t x, int32_t y) {
			WindowResizeEvent _event{ x, y };
			eventDispatcher.dispatch<WindowResizeEvent>(_event);
		});

		windowImpl.addWindowFocusCallback([&](int focused) {
			WindowFocusEvent _event{ (bool)focused };
			eventDispatcher.dispatch<WindowFocusEvent>(_event);
		});

		windowImpl.addMouseButtonCallback([&](MouseButton btn, bool pressed, int mods) {
			mButtons[btn] = pressed; 
			MouseButtonEvent _event{ btn, pressed };
			eventDispatcher.dispatch<MouseButtonEvent>(_event);	
		});

		windowImpl.addMousePosCallback([&](double x, double y) {
			MouseMoveEvent _event{ x, y };
			eventDispatcher.dispatch<MouseMoveEvent>(_event);
		});

		windowImpl.addMouseScrollCallback([&](double x, double y) {
			MouseScrollEvent _event{ y };
			eventDispatcher.dispatch<MouseScrollEvent>(_event);
		});

		windowImpl.addKeyButtonCallback([&](KeyButton btn, KeyAction action) {
			kButtons[btn] = action;
			KeyPressEvent _event{ btn, action };
			eventDispatcher.dispatch<KeyPressEvent>(_event);
		});
	}

	bool WindowInput::isMouseBtnPressed(MouseButton btn) {
		return mButtons[btn];
	}

	bool WindowInput::isKeyPressed(KeyButton btn) {
		const auto action = kButtons[btn];
		return action == KeyAction::PRESS || action == KeyAction::REPEAT;
	}

	EventDispatcher& WindowInput::getEventDispatcher() {
		return eventDispatcher;
	}

	WindowImpl& WindowInput::getWindowImpl() const {
		return windowImpl;
	}

}