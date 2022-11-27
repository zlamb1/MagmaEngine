#pragma once

#include "Vulktrix/Command/vulkan_cmd.h"

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
	VERTEX = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	INDEX = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	UNIFORM = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	TRANSFER_SRC = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	TRANSFER_DST = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT
};

inline VulkanBufferUsage operator|(VulkanBufferUsage a, VulkanBufferUsage b) {
	return (VulkanBufferUsage)((VkBufferUsageFlags)a | (VkBufferUsageFlags)b);
}

class VulkanBuffer : public VulkanObject {
	
public:
	VulkanBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice);
	~VulkanBuffer() override;

	std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;

	VkDeviceSize pSize = 0; 

	// by default buffer usage is as a vertex buffer
	VulkanBufferUsage pBufferUsageFlags = VulkanBufferUsage::VERTEX;

	VkResult init() override;

	VkMemoryRequirements queryMemRequirements();

	VkBuffer& getBuffer();

private:
	VkBuffer vkBuffer{};

};

class VulkanDeviceMemory : public VulkanObject {

public:
	VulkanDeviceMemory(std::shared_ptr<VulkanDevice> pVulkanDevice, 
		std::shared_ptr<VulkanBuffer> pVulkanBuffer);
	~VulkanDeviceMemory() override;

	std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;
	// pBuffer is used to determine requirements for the allocatated memory
	std::shared_ptr<VulkanBuffer> pVulkanBuffer = nullptr;

	// by default request memory that is visible on CPU and that is auto flushed to the GPU
	VulkanMemoryType pMemPropertyFlags = VulkanMemoryType::CPU_VISIBLE | VulkanMemoryType::FLUSH_WRITES;

	VkResult init() override;

	VkDeviceMemory& getMemory();

	VkResult bindBufferMemory(VkBuffer& vkBuffer, VkDeviceSize memoryOoffset);

	VkResult mapMemory();
	void unmapMemory();
	VkResult setData(const void* bufferData);

private:
	VkDeviceSize memorySize = 0; 
	VkDeviceMemory vkDeviceMemory{};

	void* data = nullptr;

};