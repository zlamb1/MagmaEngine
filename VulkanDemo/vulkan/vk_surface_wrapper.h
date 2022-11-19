#pragma once

#include <iostream>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "vk_logger.h"

class VkSurfaceWrapper {

public:

	VkSurfaceWrapper(GLFWwindow& _glfwWindow, VkInstance& _vkInstance);
	~VkSurfaceWrapper();

	void Initialize();

	VkSurfaceKHR& GetSurfaceKHR();

private:

	GLFWwindow& glfwWindow;
	VkInstance& vkInstance;

	VkSurfaceKHR vkSurfaceKHR{};

};