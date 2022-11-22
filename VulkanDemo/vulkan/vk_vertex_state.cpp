#include "vk_vertex_state.h"

// _VkBindingDescription Implementation

VkVertexInputBindingDescription _VkBindingDescription::getBindingDescription() {
	vkBindingDescription.binding = binding;
	vkBindingDescription.stride = stride;
	// TODO: make editable
	vkBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return vkBindingDescription;
}

// _VkAttributeDescription Implementation

VkVertexInputAttributeDescription _VkAttributeDescription::getAttributeDescription() {
	vkAttributeDescription.binding = binding;
	vkAttributeDescription.format = (VkFormat) format;
	vkAttributeDescription.location = location;
	vkAttributeDescription.offset = offset;
	return vkAttributeDescription;
}

// _VkVertexState Implementation

VkVertexInputBindingDescription _VkVertexState::getBindingDescription() {
	return pBindingDescription.getBindingDescription();
}

std::vector<VkVertexInputAttributeDescription> _VkVertexState::getAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attribDescriptions{};
	for (auto& _vkAttributeDescription : pAttributeDescriptions) {
		attribDescriptions.push_back(_vkAttributeDescription.getAttributeDescription());
	}
	return attribDescriptions;
}