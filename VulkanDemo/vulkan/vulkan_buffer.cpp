#include "vulkan_buffer.h"

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

		VulkanLogger::instance().enqueueText("findMemoryType", "Could not find suitable memory type");
		return 0;
	}
}

VulkanBuffer::VulkanBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice) :
	pVulkanDevice{ pVulkanDevice } {}

VulkanBuffer::~VulkanBuffer() {
	if (pVulkanDevice != nullptr) {
		vkFreeMemory(pVulkanDevice->getDevice(), vkBufferMemory, pAllocator);
		vkDestroyBuffer(pVulkanDevice->getDevice(), vkBuffer, pAllocator);
	}
}

VkResult VulkanBuffer::init() {
	if (pVulkanDevice == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanBuffer::init", "pVulkanDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = pSize;
	bufferCreateInfo.usage = (VkBufferUsageFlags) pBufferUsageFlags;
	// sharing between queue families
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// configures sparse buffer memory
	bufferCreateInfo.flags = 0; // optional

	auto createBufferResult = vkCreateBuffer(pVulkanDevice->getDevice(), &bufferCreateInfo, 
		pAllocator, &vkBuffer);
	VulkanLogger::instance().enqueueObject("VulkanBuffer::init::vkCreateBufferResult",
		createBufferResult);
	if (createBufferResult != VK_SUCCESS) {
		return createBufferResult;
	}

	// buffer allocation
	auto memRequirements = queryMemRequirements();

	VkMemoryAllocateInfo memAllocateInfo{};
	memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocateInfo.allocationSize = memRequirements.size;
	memAllocateInfo.memoryTypeIndex = BufferUtility::findMemoryType(pVulkanDevice->getPhysicalDevice(),
		memRequirements.memoryTypeBits, (VkMemoryPropertyFlags) pMemPropertyFlags);

	auto vkAllocateMemoryResult =
		vkAllocateMemory(pVulkanDevice->getDevice(), &memAllocateInfo, pAllocator, &vkBufferMemory);
	if (vkAllocateMemoryResult != VK_SUCCESS) {
		return vkAllocateMemoryResult;
	}
	
	return vkBindBufferMemory(pVulkanDevice->getDevice(), vkBuffer, vkBufferMemory, 0);
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

VkDeviceMemory& VulkanBuffer::getBufferMemory() {
	return vkBufferMemory;
}

VkResult VulkanBuffer::setData(const void* buffer_data) {
	void* data;
	auto vkMapMemoryResult = vkMapMemory(pVulkanDevice->getDevice(), vkBufferMemory, 0, pSize, 0, &data);
	if (vkMapMemoryResult != VK_SUCCESS) {
		return vkMapMemoryResult;
	}
	memcpy(data, buffer_data, (size_t)pSize);
	vkUnmapMemory(pVulkanDevice->getDevice(), vkBufferMemory);
	return VK_SUCCESS;
}