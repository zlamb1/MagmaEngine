#include "glfw_impl.h"

#define MakeSize(x, y) std::make_pair<int32_t, int32_t>((int32_t)x, (int32_t)y)

using namespace Window;

GLFWImpl::~GLFWImpl() {
	glfwDestroyWindow(window);
}

void GLFWImpl::init(int32_t width, int32_t height) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(width, height, "", NULL, NULL);

	glfwSetWindowUserPointer(window, this);

	glfwSetWindowSizeCallback(window, glfwOnWindowResize);
	glfwSetFramebufferSizeCallback(window, glfwOnFramebufferResize);

	glfwSetCursorPosCallback(window, glfwOnMouseMove);

	glfwSetScrollCallback(window, glfwOnMouseScroll);

	glfwSetMouseButtonCallback(window, glfwOnMousePress);

	glfwSetCursorEnterCallback(window, glfwOnMouseEnter);
}

void GLFWImpl::waitForEvents() {
	glfwWaitEvents();
}

// getters

bool GLFWImpl::shouldWindowClose() {
	return glfwWindowShouldClose(window);
}

std::pair<int32_t, int32_t> GLFWImpl::getMonitorResolution() {
	const GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return MakeSize(mode->width, mode->height);
}

std::pair<int32_t, int32_t> GLFWImpl::getSize() {
	int32_t width, height;
	glfwGetWindowSize(window, &width, &height);
	return MakeSize(width, height);
}

const std::string& GLFWImpl::getTitle() {
	return title; 
}

bool GLFWImpl::isResizable() {
	return glfwGetWindowAttrib(window, GLFW_RESIZABLE);
}

Window::Int32Size GLFWImpl::getFramebufferSize() {
	int32_t width, height;
	glfwGetFramebufferSize(window, &width, &height);
	return MakeSize(width, height);
}

// setters

void GLFWImpl::setSize(int32_t width, int32_t height) {
	glfwSetWindowSize(window, width, height);
}

void GLFWImpl::setPosition(int32_t x, int32_t y) {
	glfwSetWindowPos(window, x, y);
}

void GLFWImpl::setTitle(std::string title) {
	glfwSetWindowTitle(window, title.c_str());
	this->title = title;
}

void GLFWImpl::setResizable(bool resizable) {
	glfwSetWindowAttrib(window, GLFW_RESIZABLE, resizable);
}

void GLFWImpl::setMousePosition(double x, double y) {
	glfwSetCursorPos(window, x, y);
}

void GLFWImpl::setMouseHidden(bool hidden) {
	if (hidden) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

// polymorphic getters

MouseButton GLFWImpl::getMouseButton(int btn) {
	switch (btn) {
		case GLFW_MOUSE_BUTTON_LEFT:
			return MouseButton::LEFT;
		case GLFW_MOUSE_BUTTON_RIGHT:
			return MouseButton::RIGHT;
		default:
			return MouseButton::UNKNOWN;
	}
}

MouseAction GLFWImpl::getMouseAction(int action) {
	switch (action) {
		case GLFW_RELEASE:
			return MouseAction::RELEASE;
		case GLFW_PRESS:
			return MouseAction::PRESS;
		default:
			return MouseAction::UNKNOWN;
	}
}

// other functions

VkResult GLFWImpl::getSurfaceKHR(VkInstance& vkInstance, VkSurfaceKHR& vkSurfaceKHR) {
	return glfwCreateWindowSurface(vkInstance, window, nullptr, &vkSurfaceKHR);
}

GLFWImpl* GLFWImpl::getUserPtrFromWindow(GLFWwindow* window) {
	return (GLFWImpl*)glfwGetWindowUserPointer(window);
}

// glfw callbacks

void GLFWImpl::glfwOnWindowResize(GLFWwindow* window, int32_t width,
	int32_t height) {
	auto windowPtr = getUserPtrFromWindow(window);
	windowPtr->onWindowResize(width, height);
}

void GLFWImpl::glfwOnFramebufferResize(GLFWwindow* window, int32_t width,
	int32_t height) {
	auto windowPtr = getUserPtrFromWindow(window);
	windowPtr->onFramebufferResize(width, height);
}

void GLFWImpl::glfwOnMouseMove(GLFWwindow* window, double x, double y) {
	auto windowPtr = getUserPtrFromWindow(window);
	windowPtr->onMouseMove(x, y); 
}

void GLFWImpl::glfwOnMouseScroll(GLFWwindow* window, double x, double y) {
	auto windowPtr = getUserPtrFromWindow(window);
	windowPtr->onMouseScroll(x, y);
}

void GLFWImpl::glfwOnMousePress(GLFWwindow* window, int btn, int action, int mods) {
	auto windowPtr = getUserPtrFromWindow(window);
	int pressed = std::min(1, std::max(0, (int)windowPtr->getMouseAction(action)));
	windowPtr->onMousePress(windowPtr->getMouseButton(btn), pressed, mods);
}

void GLFWImpl::glfwOnMouseEnter(GLFWwindow* window, int entered) {
	auto windowPtr = getUserPtrFromWindow(window);
	if (entered) {
		windowPtr->onMouseEnter();
	}
	else {
		windowPtr->onMouseExit();
	}
}