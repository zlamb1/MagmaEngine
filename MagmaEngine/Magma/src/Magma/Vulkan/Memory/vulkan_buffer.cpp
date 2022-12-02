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
			vkDestroyBuffer(pVulkanDevice->getDevice(), vkBuffer, pAllocator);
		}
	}

	VkResult VulkanBuffer::init() {
		if (pVulkanDevice == nullptr) {
			Z_LOG_TXT("VulkanBuffer::init", "pVulkanDevice is nullptr");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = pSize;
		bufferCreateInfo.usage = (VkBufferUsageFlags)pBufferUsageFlags;
		// sharing between queue families
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// configures sparse buffer memory
		bufferCreateInfo.flags = 0; // optional

		auto createBufferResult = vkCreateBuffer(pVulkanDevice->getDevice(), &bufferCreateInfo,
			pAllocator, &vkBuffer);
		Z_LOG_OBJ("VulkanBuffer::init::vkCreateBufferResult", createBufferResult);

		return createBufferResult;
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

	// VulkanDeviceMemory

	VulkanDeviceMemory::VulkanDeviceMemory(std::shared_ptr<VulkanDevice> pVulkanDevice,
		std::shared_ptr<VulkanBuffer> pVulkanBuffer) :
		pVulkanDevice{ pVulkanDevice }, pVulkanBuffer{ pVulkanBuffer } {}

	VulkanDeviceMemory::~VulkanDeviceMemory() {
		if (pVulkanDevice != nullptr) {
			if (data != nullptr) {
				unmapMemory();
			}
			vkDeviceWaitIdle(pVulkanDevice->getDevice());
			vkFreeMemory(pVulkanDevice->getDevice(), vkDeviceMemory, pAllocator);
		}
	}

	VkResult VulkanDeviceMemory::init() {
		if (pVulkanDevice == nullptr) {
			Z_LOG_TXT("VulkanDeviceMemory::init", "pVulkanDevice is nullptr");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		if (pVulkanBuffer == nullptr) {
			Z_LOG_TXT("VulkanDeviceMemory::init", "pVulkanBuffer is nullptr");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		auto memRequirements = pVulkanBuffer->queryMemRequirements();
		memorySize = memRequirements.size;

		VkMemoryAllocateInfo memAllocateInfo{};
		memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocateInfo.allocationSize = memRequirements.size;
		memAllocateInfo.memoryTypeIndex = BufferUtility::findMemoryType(pVulkanDevice->getPhysicalDevice(),
			memRequirements.memoryTypeBits, (VkMemoryPropertyFlags)pMemPropertyFlags);

		auto allocateMemoryResult = vkAllocateMemory(pVulkanDevice->getDevice(), &memAllocateInfo, pAllocator,
			&vkDeviceMemory);
		Z_LOG_OBJ("VulkanBuffer::init::vkAllocateMemory", allocateMemoryResult);

		return allocateMemoryResult;
	}

	VkDeviceMemory& VulkanDeviceMemory::getMemory() {
		return vkDeviceMemory;
	}

	VkResult VulkanDeviceMemory::bindBufferMemory(VkBuffer& vkBuffer, VkDeviceSize memoryOffset) {
		if (pVulkanDevice == nullptr) {
			Z_LOG_TXT("VulkanDeviceMemory::bindBufferMemory", "pVulkanDevice is nullptr");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		return vkBindBufferMemory(pVulkanDevice->getDevice(), vkBuffer, vkDeviceMemory, memoryOffset);
	}

	VkResult VulkanDeviceMemory::mapMemory() {
		if (data != nullptr) {
			Z_LOG_TXT("VulkanDeviceMemory::mapMemory",
				"warning: attempting to map memory that is already mapped");
		}

		void* tmpData;
		auto mapMemoryResult = vkMapMemory(pVulkanDevice->getDevice(), vkDeviceMemory, 0,
			memorySize, 0, &tmpData);
		data = tmpData;
		Z_LOG_OBJ("VulkanBuffer::setData::vkMapMemory", mapMemoryResult);

		return mapMemoryResult;
	}

	void VulkanDeviceMemory::unmapMemory() {
		vkUnmapMemory(pVulkanDevice->getDevice(), vkDeviceMemory);
		data = nullptr;
	}

	VkResult VulkanDeviceMemory::setData(const void* bufferData) {
		if (data == nullptr) {
			Z_LOG_TXT("VulkanDeviceMemory::setData",
				"warning: trying to memcpy to nullptr destination")
				return VK_ERROR_INITIALIZATION_FAILED;
		}
		memcpy(data, bufferData, (size_t)memorySize);
		return VK_SUCCESS;
	}

}