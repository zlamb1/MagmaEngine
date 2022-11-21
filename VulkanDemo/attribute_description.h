#pragma once

#include <vulkan/vulkan.h>

class _VkAttributeDescription {

public:
	uint32_t binding, location, offset;
	VkFormat format; 

	VkVertexInputAttributeDescription getAttributeDescription();

private: 
	VkVertexInputAttributeDescription vkAttributeDescription;

};