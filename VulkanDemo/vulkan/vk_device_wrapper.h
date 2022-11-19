#pragma once

#include <iostream>  
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>

#include "vk_logger.h"
#include "vk_surface_wrapper.h"
#include "vk_validation_wrapper.h"

struct QueueFamily {

	std::optional<uint32_t> graphics;
	std::optional<uint32_t> presentation;

	uint32_t getGraphics() {
		return graphics.value();
	}

	uint32_t getPresentation() {
		return presentation.value();
	}

	bool isComplete() {
		return graphics.has_value() && presentation.has_value();
	}

};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats{};
	std::vector<VkPresentModeKHR> presentModes{};
};

struct _VkDevice {

	_VkDevice() {}
	~_VkDevice();

	VkInstance* pInstance = nullptr;
	VkSurfaceKHR* pSurfaceKHR = nullptr;
	_VkValidation* _pValidation = nullptr;

	VkPhysicalDevice vkPhysicalDevice{};
	VkDevice vkDevice{};

	QueueFamily vkQueueFamily{};
	VkQueue vkGraphicsQueue{}, vkPresentationQueue{};

	const std::vector<const char*> vkReqExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkResult create();
	SwapChainSupportDetails querySwapchainSupport();

};