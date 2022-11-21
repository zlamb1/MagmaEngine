#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "attribute_description.h"

struct Vertex {

	std::vector<_VkAttributeDescription> attributeDescriptions{};

	virtual VkVertexInputBindingDescription getBindingDescription();
	virtual std::vector<_VkAttributeDescription> getAttributeDescriptions();

	/* Example getAttributeDescriptions() {
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		return attributeDescriptions;
	}*/

};