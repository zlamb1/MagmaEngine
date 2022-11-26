#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_buffer.h"
#include "vulkan_object.h"

class VulkanDrawer : public VulkanObject {

public:
	VulkanDrawer() = default;
	~VulkanDrawer() override = default;

	std::vector<VulkanBuffer*> pVertexBuffers{};
	VulkanBuffer* pIndexBuffer = nullptr;

	uint32_t pVertexCount = 0, pIndexCount = 0, pInstanceCount = 1;
	uint32_t pFirstVertex = 0, pFirstInstance = 0, pFirstIndex = 0;
	int32_t pVertexOffset = 0;

	bool pUseIndexing = false;

	void onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer);

};