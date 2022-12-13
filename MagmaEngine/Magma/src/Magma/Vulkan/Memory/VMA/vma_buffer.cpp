#include "vma_buffer.h"

namespace Magma {

	namespace BufferUtility {

		static uint32_t findMemoryType(VkPhysicalDevice& vkPhysicalDevice,
			uint32_t typeFilter,
			VkMemoryPropertyFlags properties) {
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &memProperties);

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
				if ((typeFilter & (1 << i)) &&
					(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
					return i;
				}
			}

			Z_LOG_TXT("VulkanBuffer::findMemoryType", "could not find suitable memory type");
			return 0;
		}

	}

	VmaBuffer::VmaBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice) :
		VulkanBuffer(pVulkanDevice), allocator{ pVulkanDevice->getMemoryAllocator() } {}

	VmaBuffer::~VmaBuffer() {
		if (pVulkanDevice != nullptr) {
			vkDeviceWaitIdle(pVulkanDevice->getDevice());
			vmaDestroyBuffer(allocator->getAllocator(), vkBuffer, vmaAllocation);
		}
	}

	VkBuffer& VmaBuffer::getBuffer() {
		return vkBuffer;
	}

	VkMemoryRequirements VmaBuffer::queryMemRequirements() {
		VkMemoryRequirements memRequirements{};
		if (pVulkanDevice != nullptr) {
			vkGetBufferMemoryRequirements(pVulkanDevice->getDevice(), vkBuffer, &memRequirements);
		}

		return memRequirements;
	}

	VkBufferUsageFlagBits VmaBuffer::getBufferUsageFlagBits(BufferUsage bufferUsage) {
		switch (bufferUsage) {
			case BufferUsage::VERTEX:
				return VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			case BufferUsage::INDEX:
				return VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			case BufferUsage::UNIFORM:
				return VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			case BufferUsage::TRANSFER_SRC:
				return VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			case BufferUsage::TRANSFER_DST:
				return VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			default:
				return VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
	}

	void VmaBuffer::init() {
		if (pVulkanDevice == nullptr) {
			Z_LOG_TXT("VulkanBuffer::init", "pVulkanDevice is nullptr");
			return;
		}

		VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferCreateInfo.size = pSize;
		bufferCreateInfo.usage = getBufferUsageFlagBits(pBufferUsageFlags);
		// sharing between queue families
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// configures sparse buffer memory
		bufferCreateInfo.flags = 0; // optional

		// TODO: create abstract Buffer class
		// TODO: create pure Vulkan implementation + VMA implementation
		VmaAllocationCreateInfo vmaAllocInfo = {};
		vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

		auto createBufferResult = vmaCreateBuffer(allocator->getAllocator(), &bufferCreateInfo, 
			&vmaAllocInfo, &vkBuffer, &vmaAllocation, nullptr);
		Z_LOG_OBJ("VmaBuffer::init", createBufferResult);

	}

	void* VmaBuffer::getData() const {
		return m_Data; 
	}

	void VmaBuffer::setData(void* inData, size_t size) {
		if (m_Data == nullptr)
			Z_LOG_TXT("VulkanBuffer::setData", "warning: attempting to set data of unmapped buffer");
		else memcpy(m_Data, inData, size);
	}

	void VmaBuffer::map() {
		// create temp pointer as you can't copy into nullptr
		void* tmpData;
		auto mapMemoryResult = vmaMapMemory(allocator->getAllocator(), vmaAllocation, &tmpData);
		m_Data = tmpData; 
		Z_LOG_OBJ("VulkanBuffer::map", mapMemoryResult);
	}

	void VmaBuffer::unmap() {
		vmaUnmapMemory(allocator->getAllocator(), vmaAllocation);
		m_Data = nullptr; 
	}

}