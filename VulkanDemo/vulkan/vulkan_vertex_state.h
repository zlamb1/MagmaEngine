#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

enum VulkanDataFormat {
	R32_SFLOAT = VK_FORMAT_R32_SFLOAT,
	R32G32_SFLOAT = VK_FORMAT_R32G32_SFLOAT,
	R32G32B32_SFLOAT = VK_FORMAT_R32G32B32_SFLOAT,
	R32G32B32A32_SFLOAT = VK_FORMAT_R32G32B32A32_SFLOAT
};

class VulkanBindingDescription {

public:
	uint32_t binding, stride;

	VkVertexInputBindingDescription getBindingDescription();

private:
	VkVertexInputBindingDescription bindingDescription{};

};

class VulkanAttributeDescription {

public:
	uint32_t binding, location, offset;
	VulkanDataFormat format;

	VkVertexInputAttributeDescription getAttributeDescription();

private:
	VkVertexInputAttributeDescription attributeDescription{};

};

class VulkanVertexState {

public:
	virtual VkVertexInputBindingDescription getBindingDescription();
	virtual std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

protected:
	VulkanBindingDescription pBindingDescription;
	std::vector<VulkanAttributeDescription> pAttributeDescriptions;

};