#pragma once

#include <iostream>  
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>

#include "vk_surface_wrapper.h"
#include "vk_validation_wrapper.h"

struct QueueFamily {

	std::optional<uint32_t> graphics;
	std::optional<uint32_t> present;

	uint32_t getGraphics() {
		return graphics.value();
	}

	uint32_t getPresent() {
		return present.value();
	}

	bool isComplete() {
		return graphics.has_value() && present.has_value();
	}

};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes; 
};

class VkDeviceWrapper {

	public:

		VkDeviceWrapper(VkInstance& _vkInstance, 
			VkSurfaceWrapper& _vkSurfaceWrapper,
			VkValidationWrapper& _vkValidationWrapper);
		VkDeviceWrapper(VkInstance& _vkInstance,
			VkSurfaceWrapper& _vkSurfaceWrapper,
			VkValidationWrapper& _vkValidationWrapper,
			bool _debug);
		~VkDeviceWrapper();

		VkPhysicalDevice& getPhysicalDevice();
		VkDevice& getLogicalDevice();

		QueueFamily family;

		QueueFamily& getQueueFamily();

		VkQueue& getGraphicsQueue();
		VkQueue& getPresentationQueue();

		QueueFamily findQueueFamily(VkQueueFlagBits flags);
		SwapChainSupportDetails querySwapChainSupport();

	private:

		VkInstance& vkInstance;
		VkValidationWrapper& vkValidationWrapper;
		VkSurfaceWrapper& vkSurfaceWrapper;

		VkPhysicalDevice vkPhysicalDevice;
		VkDevice vkLogicalDevice;
		VkQueue vkGraphicsQueue, vkPresentQueue;

		const std::vector<const char*> requiredDeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		bool debug;

		void selectPhysicalDevice();
		bool isDeviceSuitable(VkPhysicalDevice device);
		bool checkDeviceExtensionsSupport(VkPhysicalDevice& device);
		void createLogicalDevice();

		static QueueFamily findQueueFamily(
			VkPhysicalDevice& device, 
			VkSurfaceKHR& surface, 
			VkQueueFlagBits flags);

		static SwapChainSupportDetails querySwapChainSupport(
			VkPhysicalDevice& device, VkSurfaceKHR& surface);

};