#pragma once

#include "Vulkan/Command/vulkan_cmd.h"

namespace Magma {

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

		// by default buffer usage is as a vertex buffer
		VkResult init() override;
		VkMemoryRequirements queryMemRequirements();

		VkBuffer& getBuffer();
		void* getData() const;
		
		void setData(void* nData, size_t size);

		void map();
		void unmap();

	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;
		VkDeviceSize pSize = 0;
		VulkanBufferUsage pBufferUsageFlags = VulkanBufferUsage::VERTEX;

		const MemoryAllocator const* allocator = nullptr;

	private:
		VmaAllocation vmaAllocation{};
		VkBuffer vkBuffer{};
		void* bufferData = nullptr; 

	};

}

