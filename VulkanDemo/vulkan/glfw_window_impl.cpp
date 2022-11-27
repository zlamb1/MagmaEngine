#include "glfw_window_impl.h"

#define MakeSize(x, y) std::make_pair<int32_t, int32_t>((int32_t)x, (int32_t)y)

GLFWWindowImpl::~GLFWWindowImpl() {
	glfwDestroyWindow(window);
}

void GLFWWindowImpl::init(int32_t width, int32_t height) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(width, height, "", NULL, NULL);

	glfwSetWindowUserPointer(window, this);

	glfwSetWindowSizeCallback(window, glfwOnWindowResize);
	glfwSetFramebufferSizeCallback(window, glfwOnFramebufferResize);
}

void GLFWWindowImpl::waitForEvents() {
	glfwWaitEvents();
}

// getters

std::pair<int32_t, int32_t> GLFWWindowImpl::getMonitorResolution() {
	const GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return MakeSize(mode->width, mode->height);
}

std::pair<int32_t, int32_t> GLFWWindowImpl::getSize() {
	int32_t width, height;
	glfwGetWindowSize(window, &width, &height);
	return MakeSize(width, height);
}

const std::string& GLFWWindowImpl::getTitle() {
	return title; 
}

bool GLFWWindowImpl::isResizable() {
	return glfwGetWindowAttrib(window, GLFW_RESIZABLE);
}

Window::Int32Size GLFWWindowImpl::getFramebufferSize() {
	int32_t width, height;
	glfwGetFramebufferSize(window, &width, &height);
	return MakeSize(width, height);
}

// setters

void GLFWWindowImpl::setSize(int32_t width, int32_t height) {
	glfwSetWindowSize(window, width, height);
}

void GLFWWindowImpl::setPosition(int32_t x, int32_t y) {
	glfwSetWindowPos(window, x, y);
}

void GLFWWindowImpl::setTitle(std::string title) {
	glfwSetWindowTitle(window, title.c_str());
	this->title = title;
}

void GLFWWindowImpl::setResizable(bool resizable) {
	glfwSetWindowAttrib(window, GLFW_RESIZABLE, resizable);
}

VkResult GLFWWindowImpl::getSurfaceKHR(VkInstance& vkInstance, VkSurfaceKHR& vkSurfaceKHR) {
	return glfwCreateWindowSurface(vkInstance, window, nullptr, &vkSurfaceKHR);
}

GLFWWindowImpl* GLFWWindowImpl::getUserPtrFromWindow(GLFWwindow* window) {
	return (GLFWWindowImpl*)glfwGetWindowUserPointer(window);
}

// glfw callbacks

void GLFWWindowImpl::glfwOnWindowResize(GLFWwindow* window, int32_t width, 
	int32_t height) {
	auto windowPtr = getUserPtrFromWindow(window);
	windowPtr->onWindowResize(width, height);
}

void GLFWWindowImpl::glfwOnFramebufferResize(GLFWwindow* window, int32_t width, 
	int32_t height) {
	auto windowPtr = getUserPtrFromWindow(window);
	windowPtr->onFramebufferResize(width, height);
}