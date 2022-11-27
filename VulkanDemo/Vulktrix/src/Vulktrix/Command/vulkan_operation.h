#pragma once

#include "vulkan_cmd.h"

class VulkanOperation {

public:
	VulkanOperation(std::shared_ptr<VulkanDevice> pVulkanDevice);
	virtual ~VulkanOperation() = default;

	std::shared_ptr<VulkanDevice> pVulkanDevice;
	std::shared_ptr<VulkanCmdPool> pVulkanCmdPool;

	virtual VkResult init();

protected:
	std::shared_ptr<VulkanCmdBuffer> vulkanCmdBuffer;

};