#include "vulkan_drawer.h"

namespace Magma {

	void VulkanDrawer::onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer,
		VkPipelineLayout& vkPipelineLayout) {
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
				Z_LOG_TXT("VulkanDrawer::onNewFrame", "pIndexBuffer is nullptr");
				return;
			}

			vkCmdBindIndexBuffer(vulkanCmdBuffer.getCmdBuffer(), pIndexBuffer->getBuffer(), 0,
				VkIndexType::VK_INDEX_TYPE_UINT16);

			if (pDescriptorSets.size() > 0) {
				vkCmdBindDescriptorSets(vulkanCmdBuffer.getCmdBuffer(),
					VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout,
					0, 1, &pDescriptorSets[0], 0, nullptr);
			}

			vkCmdDrawIndexed(vulkanCmdBuffer.getCmdBuffer(), pIndexCount, pInstanceCount, pFirstIndex,
				pVertexOffset, pFirstInstance);
		}
		else {
			vkCmdDraw(vulkanCmdBuffer.getCmdBuffer(), pVertexCount, pInstanceCount,
				pFirstVertex, pFirstInstance);
		}
	}

}