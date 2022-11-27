#include "window.h"

#define MakeSize(x, y) std::make_pair<int32_t, int32_t>(x, y)

void Window::init(int32_t width, int32_t height) {}
void Window::waitForEvents() {}

// getters

bool Window::shouldWindowClose() {
	return false;
}

Window::Int32Size Window::getMonitorResolution() {
	return MakeSize(0, 0);
}

Window::Int32Size Window::getSize() {
	return MakeSize(0, 0);
}

Window::Int32Size Window::getPosition() {
	return MakeSize(0, 0);
}

const std::string& Window::getTitle() {
	return "";
}

bool Window::isResizable() {
	return false;
}

Window::Int32Size Window::getFramebufferSize() {
	return MakeSize(0, 0);
}

// setters

void Window::setSize(int32_t width, int32_t height) {
	return;
}

void Window::setPosition(int32_t x, int32_t y) {
	return;
}

void Window::setTitle(std::string title) {
	return;
}

void Window::setResizable(bool resizable) {
	return;
}

// callbacks

void Window::onWindowResize(int32_t width, int32_t height) {
	for (auto& callback : windowSizeCallbacks) {
		callback(width, height);
	}
}

void Window::onFramebufferResize(int32_t width, int32_t height) {
	for (auto& callback : framebufferSizeCallbacks) {
		callback(width, height);
	}
}

void Window::addWindowSizeCallback(SizeCallback callback) {
	windowSizeCallbacks.push_back(callback);
}

void Window::addFramebufferSizeCallback(SizeCallback callback) {
	framebufferSizeCallbacks.push_back(callback);
}