#include "vk_attribute_description.h"

VkVertexInputAttributeDescription _VkAttributeDescription::getAttributeDescription() {
	VkVertexInputAttributeDescription vkAttributeDescription{};
	vkAttributeDescription.binding = binding;
	vkAttributeDescription.format = (VkFormat) format;
	vkAttributeDescription.location = location;
	vkAttributeDescription.offset = offset;
	return vkAttributeDescription;
}