#pragma once

#include <vulkan/vulkan.h>

#include "vk_logger.h"
#include "vk_obj.h"

struct _VkBuffer : VulkanWrapper {
	
	_VkBuffer();
	~_VkBuffer() override;

	VkPhysicalDevice* pPhysicalDevice = nullptr;
	VkDevice* pDevice = nullptr;
	uint32_t pSize = 0; 
	
	VkBuffer vkBuffer{};
	VkDeviceMemory vkBufferMemory{};

	VkResult create() override;

	VkResult setData(const float* buffer_data);

	VkMemoryRequirements queryMemRequirements();

};