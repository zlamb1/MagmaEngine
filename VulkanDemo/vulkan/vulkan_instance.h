#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_debugger.h"
#include "vulkan_logger.h"
#include "vulkan_object.h"
#include "vulkan_validater.h"

class VulkanInstance : public VulkanObject {

public:
	VulkanInstance(std::shared_ptr<VulkanValidater> pVulkanValidater);
	~VulkanInstance() override;

	VkDebugUtilsMessengerCreateInfoEXT pDebugCreateInfo{};
	std::shared_ptr<VulkanValidater> pVulkanValidater = nullptr;

	std::vector<const char*> pRequiredExtensions{};

	VkResult init() override; 

	VkInstance& getInstance();

private:
	VkInstance vkInstance{};

};