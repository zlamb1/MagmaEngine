#pragma once

#include <iostream>  
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_logger.h"
#include "vulkan_surface.h"
#include "vulkan_validater.h"
#include "vulkan_object.h"
#include "vulkan_instance.h"

class QueueFamily {

public:
	uint32_t getGraphics();
	uint32_t getPresentation();

	void setGraphics(uint32_t graphics);
	void setPresentation(uint32_t presentation);

	bool isComplete();

private:
	std::optional<uint32_t> graphics, presentation;

};

class SwapchainSupportDetails {

public:
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats{};
	std::vector<VkPresentModeKHR> presentModes{};

};

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

	QueueFamily& getQueueFamily();

	VkQueue& getGraphicsQueue();
	VkQueue& getPresentationQueue();

	SwapchainSupportDetails querySwapchainSupport();

private:
	VkPhysicalDevice physicalDevice{};
	VkDevice device{};

	QueueFamily queueFamily{};
	VkQueue graphicsQueue{}, presentationQueue{};

	const std::vector<const char*> reqExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

};