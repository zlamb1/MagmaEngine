#pragma once

#include <iostream>  
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>

#include "device_profile.h"

#include "vulkan_logger.h"
#include "vulkan_surface.h"
#include "vulkan_validater.h"
#include "vulkan_object.h"
#include "vulkan_instance.h"

class VulkanDevice : public VulkanObject {

public:
	VulkanDevice(std::shared_ptr<VulkanInstance> pVulkanInstance, 
		std::shared_ptr<VulkanSurface> pVulkanSurface, std::shared_ptr<VulkanValidater> pValidater);
	~VulkanDevice();

	std::shared_ptr<VulkanInstance> pVulkanInstance = nullptr;
	std::shared_ptr<VulkanSurface> pVulkanSurface = nullptr; 
	std::shared_ptr<VulkanValidater> pVulkanValidater = nullptr;

	VkResult init();

	VkPhysicalDevice& getPhysicalDevice();
	VkDevice& getDevice();

	SwapchainSupportDetails querySwapchainSupport();

private:
	VkPhysicalDevice physicalDevice{};
	VkDevice device{};

};