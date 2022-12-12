#include "vulkan_renderer.h"

namespace Magma {

	void VulkanRenderer::onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer,
		VkPipelineLayout& vkPipelineLayout) {
		VkDeviceSize offsets[] = { 0 };

		std::vector<VkBuffer> vkBuffers{};
		for (auto vulkanBuffer : pVertexBuffers) {
			if (vulkanBuffer != nullptr) {
				vkBuffers.push_back(vulkanBuffer->getBuffer());
			}
		}

		vkCmdBindVertexBuffers(vulkanCmdBuffer.getCmdBuffer(), 0, 1, vkBuffers.data(), offsets);

		if (!pDescriptorSets.empty()) {
			vkCmdBindDescriptorSets(vulkanCmdBuffer.getCmdBuffer(),
				VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout,
				0, 1, pDescriptorSets.data(), 0, nullptr);
		}

		if (useIndexing) {
			if (indexBuffer == nullptr) {
				Z_LOG_TXT("VulkanDrawer::onNewFrame", "pIndexBuffer is nullptr");
				return;
			}

			const std::shared_ptr<VulkanBuffer> _indexBuffer =
				std::dynamic_pointer_cast<VulkanBuffer>(indexBuffer);

			vkCmdBindIndexBuffer(vulkanCmdBuffer.getCmdBuffer(),
				_indexBuffer->getBuffer(), 0,
				VkIndexType::VK_INDEX_TYPE_UINT16);

			vkCmdDrawIndexed(vulkanCmdBuffer.getCmdBuffer(), indexCount, instanceCount, firstIndex,
				vertexOffset, firstInstance);
		}
		else {
			vkCmdDraw(vulkanCmdBuffer.getCmdBuffer(), vertexCount, instanceCount,
				firstVertex, firstInstance);
		}
	}

}