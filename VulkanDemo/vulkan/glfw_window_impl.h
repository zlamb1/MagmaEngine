#pragma once

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulkan_window.h"

class GLFWWindowImpl : public VulkanWindow {

public:

	GLFWWindowImpl() = default;
	~GLFWWindowImpl() override;

	void init(int32_t width, int32_t height) override;
	void waitForEvents() override;

	// getters
	Window::Int32Size getMonitorResolution() override;
	Window::Int32Size getSize() override;
	const std::string& getTitle() override;  
	bool isResizable() override;
	Window::Int32Size getFramebufferSize() override;

	// setters
	void setResizable(bool resizable) override;
	void setSize(int32_t width, int32_t height) override;
	void setPosition(int32_t x, int32_t y) override;
	void setTitle(std::string title) override;

	// Vulkan surface getter
	VkResult getSurfaceKHR(VkInstance& vkInstance, 
		VkSurfaceKHR& vkSurfaceKHR) override;

private:
	// window pointer
	GLFWwindow* window = nullptr; 

	// glfw doesn't store title
	std::string title;

	static GLFWWindowImpl* getUserPtrFromWindow(GLFWwindow* window);

	// static callbacks for GLFW
	static void glfwOnWindowResize(GLFWwindow* window, int32_t width, int32_t height);
	static void glfwOnFramebufferResize(GLFWwindow* window, int32_t width, int32_t height);

};