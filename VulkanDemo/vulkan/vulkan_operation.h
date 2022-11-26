#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_cmd.h"

class VulkanOperation {

public:
	VulkanOperation() = default; 
	virtual ~VulkanOperation();

	VulkanDevice* pDevice = nullptr;

	virtual VkResult init();

	void setCmdPool(VulkanCmdPool* pCmdPool);

protected:
	VulkanCmdPool* pCmdPool = nullptr;
	VulkanCmdBuffer* vulkanCmdBuffer = nullptr;

	bool cmdPoolSet = false; 

};