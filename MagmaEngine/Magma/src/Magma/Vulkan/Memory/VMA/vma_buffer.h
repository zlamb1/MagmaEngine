#pragma once

#include "Vulkan/Memory/vulkan_buffer.h"

#include "Vulkan/Command/vulkan_cmd.h"

namespace Magma {

	class VmaBuffer : public VulkanBuffer {

	public:
		VmaBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice);

		VkBuffer& getBuffer() override;
		VkMemoryRequirements queryMemRequirements() override;
		VkBufferUsageFlagBits getBufferUsageFlagBits(BufferUsage bufferUsage) override;

	public:
		~VmaBuffer();

		void init() override;

		void* getData() const override;
		void setData(void* nData, size_t size) override; 

		void map() override;
		void unmap() override;

	public:
		const MemoryAllocator const* allocator = nullptr;

	private:
		VmaAllocation vmaAllocation{};
		VkBuffer vkBuffer{};

	};

}

