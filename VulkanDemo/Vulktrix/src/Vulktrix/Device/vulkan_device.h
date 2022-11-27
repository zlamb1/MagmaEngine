#pragma once

#include <iostream>  
#include <stdexcept>

#include "device_profile.h"

#include "Vulktrix/Logging/vulkan_validater.h"

#include "Vulktrix/Setup/vulkan_instance.h"

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