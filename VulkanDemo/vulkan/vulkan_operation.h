#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_cmd.h"

class VulkanOperation {

public:
	VulkanOperation(VulkanDevice& pDevice);
	virtual ~VulkanOperation() = default;

	VulkanDevice& pDevice;
	VulkanCmdPool pCmdPool{};

	virtual VkResult init();

protected:
	VulkanCmdBuffer vulkanCmdBuffer;

};