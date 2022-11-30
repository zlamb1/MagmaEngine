#include "window.h"

#define MakeSize(x, y) std::make_pair<int32_t, int32_t>(x, y)

using namespace Window;

// main functions

void WindowImpl::init(int32_t width, int32_t height) {}

void WindowImpl::waitForEvents() {}

// getters

bool WindowImpl::shouldWindowClose() {
	return false;
}

Window::Int32Size WindowImpl::getMonitorResolution() {
	return MakeSize(0, 0);
}

Window::Int32Size WindowImpl::getSize() {
	return MakeSize(0, 0);
}

Window::Int32Size WindowImpl::getPosition() {
	return MakeSize(0, 0);
}

const std::string& WindowImpl::getTitle() {
	return "";
}

bool WindowImpl::isResizable() {
	return false;
}

Int32Size WindowImpl::getFramebufferSize() {
	return MakeSize(0, 0);
}

// setters

void WindowImpl::setSize(int32_t width, int32_t height) {}

void WindowImpl::setPosition(int32_t x, int32_t y) {}

void WindowImpl::setTitle(std::string title) {}

void WindowImpl::setResizable(bool resizable) {}

void WindowImpl::setMousePosition(double x, double y) {}

void WindowImpl::setMouseHidden(bool hidden) {}

// callbacks

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

void WindowImpl::onMousePress(Window::MouseButton button, bool pressed, int mods) {
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

// callback adders

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

// polymorphic getters
MouseButton WindowImpl::getMouseButton(int btn) {
	return MouseButton::UNKNOWN;
}

MouseAction WindowImpl::getMouseAction(int action) {
	return MouseAction::RELEASE;
}