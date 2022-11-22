#include "vk_buffer.h"

namespace _VkBufferUtility {
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

		auto _vkLogger = _VkLogger::Instance();
		_vkLogger.LogText("findMemoryType => Could not find suitable memory type!");

		return 0;
	}
}

_VkBuffer::_VkBuffer() {

}

_VkBuffer::~_VkBuffer() {
	if (pDevice != nullptr) {
		vkDestroyBuffer(*pDevice, vkBuffer, pAllocator);
		vkFreeMemory(*pDevice, vkBufferMemory, pAllocator);
	}
}

VkResult _VkBuffer::create() {
	auto _vkLogger = _VkLogger::Instance();

	if (pPhysicalDevice == nullptr) {
		_vkLogger.LogText("_VkBuffer => pPhysicalDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pDevice == nullptr) {
		_vkLogger.LogText("_VkBuffer => pDevice is nullptr");
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

	auto vkCreateBufferResult = vkCreateBuffer(*pDevice, &bufferInfo, pAllocator, &vkBuffer);
	if (vkCreateBufferResult != VK_SUCCESS) {
		_vkLogger.LogResult("_VkBuffer =>", vkCreateBufferResult);
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	// buffer allocation
	auto memRequirements = queryMemRequirements();

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = _VkBufferUtility::findMemoryType(*pPhysicalDevice,
		memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	auto vkAllocateMemoryResult =
		vkAllocateMemory(*pDevice, &allocInfo, pAllocator, &vkBufferMemory);
	if (vkAllocateMemoryResult != VK_SUCCESS) {
		return vkAllocateMemoryResult;
	}
	
	return vkBindBufferMemory(*pDevice, vkBuffer, vkBufferMemory, 0);
}

VkResult _VkBuffer::setData(const float* buffer_data) {
	auto _vkLogger = _VkLogger::Instance();

	if (pDevice == nullptr) {
		_vkLogger.LogText("_VkBuffer:setData => pDevice is nullptr");
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

VkMemoryRequirements _VkBuffer::queryMemRequirements() {
	VkMemoryRequirements memRequirements{};
	if (pDevice != nullptr) {
		vkGetBufferMemoryRequirements(*pDevice, vkBuffer, &memRequirements);
	}
	return memRequirements; 
}