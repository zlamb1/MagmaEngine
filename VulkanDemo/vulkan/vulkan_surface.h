#pragma once

#include <iostream>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "vulkan_logger.h"
#include "vulkan_object.h"

class VulkanSurface : public VulkanObject {

public:
	VulkanSurface() = default;
	~VulkanSurface() override;

	GLFWwindow* pWindow;
	VkInstance* pInstance;

	VkResult init() override;

	VkSurfaceKHR& getSurfaceKHR();

private:
	VkSurfaceKHR vkSurfaceKHR;

};