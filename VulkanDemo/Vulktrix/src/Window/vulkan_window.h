#pragma once

#include <vulkan/vulkan.h>

#include "window.h"

class VulkanWindow : public Window {

public:
	virtual VkResult getSurfaceKHR(VkInstance& vkInstance, 
		VkSurfaceKHR& vkSurfaceKHR) {
		return VK_SUCCESS;
	}

};