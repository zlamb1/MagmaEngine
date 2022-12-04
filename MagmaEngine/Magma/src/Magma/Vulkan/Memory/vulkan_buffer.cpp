#include "vulkan_buffer.h"

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

	VulkanBuffer::VulkanBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice) :
		pVulkanDevice{ pVulkanDevice } {}

	VulkanBuffer::~VulkanBuffer() {
		if (pVulkanDevice != nullptr) {
			vkDeviceWaitIdle(pVulkanDevice->getDevice());
			vmaDestroyBuffer(allocator->getAllocator(), vkBuffer, vmaAllocation);
		}
	}

	VkResult VulkanBuffer::init() {
		if (pVulkanDevice == nullptr) {
			Z_LOG_TXT("VulkanBuffer::init", "pVulkanDevice is nullptr");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		allocator = pVulkanDevice->getMemoryAllocator();

		VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferCreateInfo.size = pSize;
		bufferCreateInfo.usage = (VkBufferUsageFlags) pBufferUsageFlags;
		// sharing between queue families
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// configures sparse buffer memory
		bufferCreateInfo.flags = 0; // optional

		// TODO: create abstract Buffer class
		// TODO: create pure Vulkan implementation + VMA implementation
		VmaAllocationCreateInfo vmaAllocInfo = {};
		vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

		return vmaCreateBuffer(allocator->getAllocator(), &bufferCreateInfo, &vmaAllocInfo,
			&vkBuffer, &vmaAllocation, nullptr);
	}

	VkMemoryRequirements VulkanBuffer::queryMemRequirements() {
		VkMemoryRequirements memRequirements{};
		if (pVulkanDevice != nullptr) {
			vkGetBufferMemoryRequirements(pVulkanDevice->getDevice(), vkBuffer, &memRequirements);
		}

		return memRequirements;
	}

	VkBuffer& VulkanBuffer::getBuffer() {
		return vkBuffer;
	}

	void* VulkanBuffer::getData() const {
		return bufferData; 
	}

	void VulkanBuffer::setData(void* data, size_t size) {
		if (bufferData == nullptr) {
			Z_LOG_TXT("VulkanBuffer::setData", "warning: attempting to set data of unmapped buffer");
		}
		else memcpy(bufferData, data, size);
	}

	void VulkanBuffer::map() {
		void* tmpData;
		auto mapMemoryResult = vmaMapMemory(allocator->getAllocator(), vmaAllocation, &tmpData);
		bufferData = tmpData; 
		Z_LOG_OBJ("VulkanBuffer::map", mapMemoryResult);
	}

	void VulkanBuffer::unmap() {
		vmaUnmapMemory(allocator->getAllocator(), vmaAllocation);
		bufferData = nullptr; 
	}

}