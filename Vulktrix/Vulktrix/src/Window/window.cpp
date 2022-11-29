#include "window.h"

#define MakeSize(x, y) std::make_pair<int32_t, int32_t>(x, y)

void Window::WindowImpl::init(int32_t width, int32_t height) {}

void Window::WindowImpl::waitForEvents() {}

// getters

bool Window::WindowImpl::shouldWindowClose() {
	return false;
}

Window::Int32Size Window::WindowImpl::getMonitorResolution() {
	return MakeSize(0, 0);
}

Window::Int32Size Window::WindowImpl::getSize() {
	return MakeSize(0, 0);
}

Window::Int32Size Window::WindowImpl::getPosition() {
	return MakeSize(0, 0);
}

const std::string& Window::WindowImpl::getTitle() {
	return "";
}

bool Window::WindowImpl::isResizable() {
	return false;
}

Window::Int32Size Window::WindowImpl::getFramebufferSize() {
	return MakeSize(0, 0);
}

// setters

void Window::WindowImpl::setSize(int32_t width, int32_t height) {}

void Window::WindowImpl::setPosition(int32_t x, int32_t y) {}

void Window::WindowImpl::setTitle(std::string title) {}

void Window::WindowImpl::setResizable(bool resizable) {}

void Window::WindowImpl::setMousePosition(double x, double y) {}

void Window::WindowImpl::setMouseHidden(bool hidden) {}

// callbacks

void Window::WindowImpl::onWindowResize(int32_t width, int32_t height) {
	for (auto& callback : windowSizeCallbacks) {
		callback(width, height);
	}
}

void Window::WindowImpl::onFramebufferResize(int32_t width, int32_t height) {
	for (auto& callback : framebufferSizeCallbacks) {
		callback(width, height);
	}
}

void Window::WindowImpl::onMouseMove(int32_t x, int32_t y) {
	for (auto& callback : mousePosCallbacks) {
		callback(x, y);
	}
}

void Window::WindowImpl::onMouseScroll(double x, double y) {
	for (auto& callback : mouseScrollCallbacks) {
		callback(x, y);
	}
}

void Window::WindowImpl::onMousePress(Window::MouseButton button, Window::MouseAction action, int mods) {
	for (auto& callback : mouseButtonCallbacks) {
		callback(button, action, mods);
	}
}

void Window::WindowImpl::onMouseEnter() {
	for (auto& callback : mouseEnterCallbacks) {
		callback();
	}
}

void Window::WindowImpl::onMouseExit() {
	for (auto& callback : mouseExitCallbacks) {
		callback();
	}
}

// callback adders

void Window::WindowImpl::addWindowSizeCallback(SizeCallback callback) {
	windowSizeCallbacks.push_back(callback);
}

void Window::WindowImpl::addFramebufferSizeCallback(SizeCallback callback) {
	framebufferSizeCallbacks.push_back(callback);
}

void Window::WindowImpl::addMousePosCallback(PosCallback callback) {
	mousePosCallbacks.push_back(callback);
}

void Window::WindowImpl::addMouseScrollCallback(OffsetCallback callback) {
	mouseScrollCallbacks.push_back(callback);
}

void Window::WindowImpl::addMouseButtonCallback(ButtonCallback callback) {
	mouseButtonCallbacks.push_back(callback);
}

void Window::WindowImpl::addMouseEnterCallback(VoidCallback callback) {
	mouseEnterCallbacks.push_back(callback);
}

void Window::WindowImpl::addMouseExitCallback(VoidCallback callback) {
	mouseExitCallbacks.push_back(callback);
}