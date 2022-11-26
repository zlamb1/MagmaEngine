#include "vulkan_drawer.h"

void VulkanDrawer::onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer) {
	VkDeviceSize offsets[] = { 0 };

	std::vector<VkBuffer> vkBuffers{};
	for (auto vulkanBuffer : pVertexBuffers) {
		if (vulkanBuffer != nullptr) {
			vkBuffers.push_back(vulkanBuffer->getBuffer());
		}
	}

	vkCmdBindVertexBuffers(vulkanCmdBuffer.getCmdBuffer(), 0, 1, vkBuffers.data(), offsets);

	if (pUseIndexing) {
		if (pIndexBuffer == nullptr) {
			VulkanLogger::instance().enqueueText("VulkanDrawer::onNewFrame", "pIndexBuffer is nullptr");
			return;
		}

		vkCmdBindIndexBuffer(vulkanCmdBuffer.getCmdBuffer(), pIndexBuffer->getBuffer(), 0, 
			VkIndexType::VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(vulkanCmdBuffer.getCmdBuffer(), pIndexCount, pInstanceCount, pFirstIndex, 
			pVertexOffset, pFirstInstance);
	}
	else {
		vkCmdDraw(vulkanCmdBuffer.getCmdBuffer(), pVertexCount, pInstanceCount, 
			pFirstVertex, pFirstInstance);
	} 
}