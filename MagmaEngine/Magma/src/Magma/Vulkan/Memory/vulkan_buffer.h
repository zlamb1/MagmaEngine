#pragma once

#include "Magma/Render/buffer.h"

#include "Magma/Vulkan/Device/vulkan_device.h"

namespace Magma {

	class VulkanBuffer : public Buffer {

	public:
		VulkanBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice) :
			pVulkanDevice{ pVulkanDevice } {};

		virtual VkBuffer& getBuffer() = 0;
		virtual VkMemoryRequirements queryMemRequirements() = 0;

		virtual VkBufferUsageFlagBits getBufferUsageFlagBits(BufferUsage bufferUsage) = 0;

	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;
		VkDeviceSize pSize = 0; 
		BufferUsage pBufferUsageFlags = BufferUsage::VERTEX;

	protected:
		VkBuffer vkBuffer{};

	};

}