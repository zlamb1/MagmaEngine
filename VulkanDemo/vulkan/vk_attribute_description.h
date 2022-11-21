#pragma once

#include <vulkan/vulkan.h>

enum _VkFormat {
	R32_SFLOAT = VK_FORMAT_R32_SFLOAT,
	R32G32_SFLOAT = VK_FORMAT_R32G32_SFLOAT,
	R32G32B32_SFLOAT = VK_FORMAT_R32G32B32_SFLOAT,
	R32G32B32A32_SFLOAT = VK_FORMAT_R32G32B32A32_SFLOAT
};

class _VkAttributeDescription {

public:
	uint32_t binding, location, offset;
	_VkFormat format; 

	VkVertexInputAttributeDescription getAttributeDescription();

private: 
	VkVertexInputAttributeDescription vkAttributeDescription;

};