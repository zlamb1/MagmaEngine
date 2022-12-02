#pragma once

#include <iostream>  
#include <stdexcept>

#include "device_profile.h"

#include "Vulkan/Setup/vulkan_validater.h"
#include "Vulkan/Setup/vulkan_instance.h"

namespace Magma {
	
	class VulkanDevice : public VulkanObject {

	public:
		VulkanDevice(std::shared_ptr<VulkanInstance> pVulkanInstance,
			std::shared_ptr<VulkanSurface> pVulkanSurface, std::shared_ptr<VulkanValidater> pValidater);
		~VulkanDevice();

		VkResult init();

		VkPhysicalDevice& getPhysicalDevice();
		VkDevice& getDevice();

		SwapchainSupportDetails querySwapchainSupport();

	public:
		std::shared_ptr<VulkanInstance> pVulkanInstance = nullptr;
		std::shared_ptr<VulkanSurface> pVulkanSurface = nullptr;
		std::shared_ptr<VulkanValidater> pVulkanValidater = nullptr;

	private:
		VkPhysicalDevice physicalDevice{};
		VkDevice device{};

	};

}