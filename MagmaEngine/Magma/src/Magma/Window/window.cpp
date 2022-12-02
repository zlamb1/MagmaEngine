#include "window.h"

#define MakeSize(x, y) std::make_pair<int32_t, int32_t>(x, y)

namespace Magma {

	void WindowImpl::onWindowResize(int32_t width, int32_t height) {
		for (auto& callback : windowSizeCallbacks) {
			callback(width, height);
		}
	}

	void WindowImpl::onFramebufferResize(int32_t width, int32_t height) {
		for (auto& callback : framebufferSizeCallbacks) {
			callback(width, height);
		}
	}

	void WindowImpl::onMouseMove(int32_t x, int32_t y) {
		for (auto& callback : mousePosCallbacks) {
			callback(x, y);
		}
	}

	void WindowImpl::onMouseScroll(double x, double y) {
		for (auto& callback : mouseScrollCallbacks) {
			callback(x, y);
		}
	}

	void WindowImpl::onMousePress(MouseButton button, bool pressed, int mods) {
		for (auto& callback : mouseButtonCallbacks) {
			callback(button, pressed, mods);
		}
	}

	void WindowImpl::onMouseEnter() {
		for (auto& callback : mouseEnterCallbacks) {
			callback();
		}
	}

	void WindowImpl::onMouseExit() {
		for (auto& callback : mouseExitCallbacks) {
			callback();
		}
	}

	void WindowImpl::addWindowSizeCallback(SizeCallback callback) {
		windowSizeCallbacks.push_back(callback);
	}

	void WindowImpl::addFramebufferSizeCallback(SizeCallback callback) {
		framebufferSizeCallbacks.push_back(callback);
	}

	void WindowImpl::addMousePosCallback(PosCallback callback) {
		mousePosCallbacks.push_back(callback);
	}

	void WindowImpl::addMouseScrollCallback(OffsetCallback callback) {
		mouseScrollCallbacks.push_back(callback);
	}

	void WindowImpl::addMouseButtonCallback(ButtonCallback callback) {
		mouseButtonCallbacks.push_back(callback);
	}

	void WindowImpl::addMouseEnterCallback(VoidCallback callback) {
		mouseEnterCallbacks.push_back(callback);
	}

	void WindowImpl::addMouseExitCallback(VoidCallback callback) {
		mouseExitCallbacks.push_back(callback);
	}

	MouseButton WindowImpl::getMouseButton(int btn) {
		return MouseButton::UNKNOWN;
	}

	MouseAction WindowImpl::getMouseAction(int action) {
		return MouseAction::RELEASE;
	}

}

