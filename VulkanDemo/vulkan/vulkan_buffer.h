#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_logger.h"
#include "vulkan_object.h"

// I'm not using scoped enums here to maintain the implicit conversion capability of regular enums
namespace VulkanMemoryType {
	enum VulkanMemoryType {
		GPU_VISIBLE = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		CPU_VISIBLE = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
}

namespace VulkanBufferUsage {
	enum VulkanBufferUsage {
		VERTEX = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		USAGE_TRANSFER_SRC = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		USAGE_TRANSFER_DST = VK_BUFFER_USAGE_TRANSFER_DST_BIT
	};
}


class VulkanBuffer : public VulkanObject {
	
public:
	VulkanBuffer() = default;
	~VulkanBuffer() override;

	VkPhysicalDevice* pPhysicalDevice = nullptr;
	VkDevice* pDevice = nullptr;
	uint32_t pSize = 0; 

	// by default buffer usage is as a vertex buffer
	VkBufferUsageFlags pBufferUsageFlags = VulkanBufferUsage::VERTEX;

	// by default request memory that is accessible on the GPU and CPU
	VkMemoryPropertyFlags pMemPropertyFlags = VulkanMemoryType::CPU_VISIBLE | 
		VulkanMemoryType::GPU_VISIBLE;

	VkResult init() override;

	VkBuffer& getBuffer();
	VkDeviceMemory& getBufferMemory();

	VkResult setData(const void* buffer_data);
	VkMemoryRequirements queryMemRequirements();

private:
	VkBuffer vkBuffer{};
	VkDeviceMemory vkBufferMemory{};

};