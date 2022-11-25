#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_logger.h"
#include "vulkan_object.h"

class VulkanBuffer : public VulkanObject {
	
public:
	VulkanBuffer() = default;
	~VulkanBuffer() override;

	VkPhysicalDevice* pPhysicalDevice = nullptr;
	VkDevice* pDevice = nullptr;
	uint32_t pSize = 0; 

	VkResult init() override;

	VkBuffer& getBuffer();
	VkDeviceMemory& getBufferMemory();

	VkResult setData(const void* buffer_data);
	VkMemoryRequirements queryMemRequirements();

private:
	VkBuffer vkBuffer{};
	VkDeviceMemory vkBufferMemory{};

};