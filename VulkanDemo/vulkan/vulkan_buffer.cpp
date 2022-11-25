#include "vulkan_buffer.h"

namespace buffer_utility {
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

VulkanBuffer::~VulkanBuffer() {
	if (pDevice != nullptr) {
		vkDestroyBuffer(*pDevice, vkBuffer, pAllocator);
		vkFreeMemory(*pDevice, vkBufferMemory, pAllocator);
	}
}

VkResult VulkanBuffer::init() {
	if (pPhysicalDevice == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanBuffer::init", "pPhysicalDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pDevice == nullptr) {
		VulkanLogger::instance().enqueueObject("VulkanBuffer::init", "pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED; 
	}

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = pSize;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	// sharing between queue families
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// configures sparse buffer memory
	bufferInfo.flags = 0; // optional

	auto createBufferResult = vkCreateBuffer(*pDevice, &bufferInfo, pAllocator, &vkBuffer);
	if (createBufferResult != VK_SUCCESS) {
		VulkanLogger::instance().enqueueObject("VulkanBuffer::init::vkCreateBufferResult", 
			createBufferResult);
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	// buffer allocation
	auto memRequirements = queryMemRequirements();

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = buffer_utility::findMemoryType(*pPhysicalDevice,
		memRequirements.memoryTypeBits, pMemPropertyFlags);

	auto vkAllocateMemoryResult =
		vkAllocateMemory(*pDevice, &allocInfo, pAllocator, &vkBufferMemory);
	if (vkAllocateMemoryResult != VK_SUCCESS) {
		return vkAllocateMemoryResult;
	}
	
	return vkBindBufferMemory(*pDevice, vkBuffer, vkBufferMemory, 0);
}

VkBuffer& VulkanBuffer::getBuffer() {
	return vkBuffer;
}

VkDeviceMemory& VulkanBuffer::getBufferMemory() {
	return vkBufferMemory;
}

VkResult VulkanBuffer::setData(const void* buffer_data) {
	if (pDevice == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanBuffer::setData", "pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	void* data;
	auto vkMapMemoryResult = vkMapMemory(*pDevice, vkBufferMemory, 0, pSize, 0, &data);
	if (vkMapMemoryResult != VK_SUCCESS) {
		return vkMapMemoryResult;
	}
	memcpy(data, buffer_data, (size_t)pSize);
	vkUnmapMemory(*pDevice, vkBufferMemory);
	return VK_SUCCESS;
}

VkMemoryRequirements VulkanBuffer::queryMemRequirements() {
	VkMemoryRequirements memRequirements{};
	if (pDevice != nullptr) {
		vkGetBufferMemoryRequirements(*pDevice, vkBuffer, &memRequirements);
	}
	return memRequirements; 
}