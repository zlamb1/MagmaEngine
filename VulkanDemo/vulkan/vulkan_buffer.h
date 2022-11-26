#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_logger.h"
#include "vulkan_object.h"
#include "vulkan_cmd.h"

enum class VulkanMemoryType {
	GPU_EFFICIENT = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	CPU_VISIBLE = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
	FLUSH_WRITES = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	CPU_CACHED = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
	GPU_ONLY = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT
};

inline VulkanMemoryType operator|(VulkanMemoryType a, VulkanMemoryType b) {
	return (VulkanMemoryType)((VkMemoryPropertyFlags)a | (VkMemoryPropertyFlags)b);
}

enum class VulkanBufferUsage {
	VERTEX = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	TRANSFER_SRC = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	TRANSFER_DST = VK_BUFFER_USAGE_TRANSFER_DST_BIT
};

inline VulkanBufferUsage operator|(VulkanBufferUsage a, VulkanBufferUsage b) {
	return (VulkanBufferUsage)((VkBufferUsageFlags)a | (VkBufferUsageFlags)b);
}

class VulkanBuffer : public VulkanObject {
	
public:
	VulkanBuffer() = default;
	~VulkanBuffer() override;

	VkPhysicalDevice* pPhysicalDevice = nullptr;
	VkDevice* pDevice = nullptr;
	uint32_t pSize = 0; 

	// by default buffer usage is as a vertex buffer
	VulkanBufferUsage pBufferUsageFlags = VulkanBufferUsage::VERTEX;

	// by default request memory that is visible on CPU and that auto flushes
	VulkanMemoryType pMemPropertyFlags = VulkanMemoryType::CPU_VISIBLE | 
		VulkanMemoryType::FLUSH_WRITES;

	VkResult init() override;

	VkBuffer& getBuffer();
	VkDeviceMemory& getBufferMemory();

	VkResult setData(const void* buffer_data);
	VkMemoryRequirements queryMemRequirements();

private:
	VkBuffer vkBuffer{};
	VkDeviceMemory vkBufferMemory{};

};