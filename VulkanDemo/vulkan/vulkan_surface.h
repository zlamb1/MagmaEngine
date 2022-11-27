#pragma once

#include <iostream>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include "vulkan_window.h"

#include "vulkan_logger.h"
#include "vulkan_object.h"
#include "vulkan_instance.h"

class VulkanSurface : public VulkanObject {

public:
	VulkanSurface(VulkanWindow& pWindowImpl,
		std::shared_ptr<VulkanInstance> pVulkanInstance);
	~VulkanSurface() override;

	VulkanWindow& pWindowImpl;
	std::shared_ptr<VulkanInstance> pVulkanInstance;

	VkResult init() override;

	VkSurfaceKHR& getSurfaceKHR();

private:
	VkSurfaceKHR vkSurfaceKHR{};

};