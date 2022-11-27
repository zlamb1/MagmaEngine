#include "vulkan_vertex_state.h"

// VulkanBindingDescription

VkVertexInputBindingDescription VulkanBindingDescription::getBindingDescription() {
	bindingDescription.binding = binding;
	bindingDescription.stride = stride;
	// TODO: make editable
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}

// VulkanAttributeDescription

VkVertexInputAttributeDescription VulkanAttributeDescription::getAttributeDescription() {
	attributeDescription.binding = binding;
	attributeDescription.format = (VkFormat) format;
	attributeDescription.location = location;
	attributeDescription.offset = offset;
	return attributeDescription;
}

// VulkanVertexState

VkVertexInputBindingDescription VulkanVertexState::getBindingDescription() {
	return pBindingDescription.getBindingDescription();
}

std::vector<VkVertexInputAttributeDescription> VulkanVertexState::getAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attribDescriptions{};
	for (auto& _vkAttributeDescription : pAttributeDescriptions) {
		attribDescriptions.push_back(_vkAttributeDescription.getAttributeDescription());
	}
	return attribDescriptions;
}