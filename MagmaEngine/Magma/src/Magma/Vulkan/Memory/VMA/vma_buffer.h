#pragma once

#include "Magma/Vulkan/Memory/vulkan_buffer.h"

#include "Magma/Vulkan/Command/vulkan_cmd.h"

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

		[[nodiscard]] uint64_t getSize() const override; 
		
		[[nodiscard]] void* get() const override;
		void set(void* nData, size_t size) override; 

		void map() override;
		void unmap() override;

	public:
		const MemoryAllocator* const allocator;

	private:
		VmaAllocation vmaAllocation{};
		VkBuffer vkBuffer{};

	};

}

