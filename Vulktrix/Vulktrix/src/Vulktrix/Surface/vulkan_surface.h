#pragma once

#include <iostream>
#include <stdexcept>

#include "Window/vulkan_window.h"

#include "Vulktrix/Logging/vulkan_logger.h"

#include "Vulktrix/Setup/vulkan_instance.h"

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