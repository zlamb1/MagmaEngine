#pragma once

#include <iostream>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "vulkan_logger.h"
#include "vulkan_object.h"
#include "vulkan_instance.h"

class VulkanSurface : public VulkanObject {

public:
	VulkanSurface(GLFWwindow* pWindow, std::shared_ptr<VulkanInstance> pVulkanInstance);
	~VulkanSurface() override;

	GLFWwindow* pWindow;
	std::shared_ptr<VulkanInstance> pVulkanInstance;

	VkResult init() override;

	VkSurfaceKHR& getSurfaceKHR();

private:
	VkSurfaceKHR vkSurfaceKHR{};

};