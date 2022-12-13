#include "window.h"

#define MakeSize(x, y) std::make_pair<int32_t, int32_t>(x, y)

namespace Magma {

	void WindowImpl::onWindowResize(int32_t width, int32_t height) {
		for (auto& callback : m_WindowSizeCallbacks) {
			callback(width, height);
		}
	}

	void WindowImpl::onWindowFocus(int focused) {
		for (auto& callback : m_WindowFocusCallbacks) {
			callback(focused);
		}
	}

	void WindowImpl::onFramebufferResize(int32_t width, int32_t height) {
		for (auto& callback : m_FramebufferSizeCallbacks) {
			callback(width, height);
		}
	}

	void WindowImpl::onMouseMove(int32_t x, int32_t y) {
		for (auto& callback : m_MousePosCallbacks) {
			callback(x, y);
		}
	}

	void WindowImpl::onMouseScroll(double x, double y) {
		for (auto& callback : m_MouseScrollCallbacks) {
			callback(x, y);
		}
	}

	void WindowImpl::onMousePress(MouseButton button, bool pressed, int mods) {
		for (auto& callback : m_MouseButtonCallbacks) {
			callback(button, pressed, mods);
		}
	}

	void WindowImpl::onMouseEnter() {
		for (auto& callback : m_MouseEnterCallbacks) {
			callback();
		}
	}

	void WindowImpl::onMouseExit() {
		for (auto& callback : m_MouseExitCallbacks) {
			callback();
		}
	}

	void WindowImpl::onKeyPress(KeyButton button, KeyAction action) {
		for (auto& callback : m_KeyPressCallbacks) {
			callback(button, action);
		}
	}

	void WindowImpl::addWindowSizeCallback(SizeCallback callback) {
		m_WindowSizeCallbacks.push_back(callback);
	}

	void WindowImpl::addWindowFocusCallback(BoolCallback callback) {
		m_WindowFocusCallbacks.push_back(callback);
	}

	void WindowImpl::addFramebufferSizeCallback(SizeCallback callback) {
		m_FramebufferSizeCallbacks.push_back(callback);
	}

	void WindowImpl::addMousePosCallback(PosCallback callback) {
		m_MousePosCallbacks.push_back(callback);
	}

	void WindowImpl::addMouseScrollCallback(OffsetCallback callback) {
		m_MouseScrollCallbacks.push_back(callback);
	}

	void WindowImpl::addMouseButtonCallback(MouseButtonCallback callback) {
		m_MouseButtonCallbacks.push_back(callback);
	}

	void WindowImpl::addKeyButtonCallback(KeyPressCallback callback) {
		m_KeyPressCallbacks.push_back(callback);
	}

	void WindowImpl::addMouseEnterCallback(VoidCallback callback) {
		m_MouseEnterCallbacks.push_back(callback);
	}

	void WindowImpl::addMouseExitCallback(VoidCallback callback) {
		m_MouseExitCallbacks.push_back(callback);
	}

	MouseButton WindowImpl::getMouseButton(int btn) {
		return MouseButton::UNKNOWN;
	}

	MouseAction WindowImpl::getMouseAction(int action) {
		return MouseAction::RELEASE;
	}

}

