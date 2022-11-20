#pragma once

#include <iostream>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "vk_logger.h"
#include "vk_obj.h"

struct _VkSurface : VulkanWrapper {

	_VkSurface();
	~_VkSurface();

	GLFWwindow* pWindow;
	VkInstance* pInstance;

	VkSurfaceKHR vkSurfaceKHR;

	VkResult create();

};