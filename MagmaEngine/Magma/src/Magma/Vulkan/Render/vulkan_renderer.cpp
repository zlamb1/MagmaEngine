#include "vulkan_renderer.h"

namespace Magma {

	void VulkanRenderer::onNewFrame(VulkanCmdBuffer& commandBuffer,
		const VkPipelineLayout& pipelineLayout) const {
		constexpr VkDeviceSize offsets[] = { 0 };

		std::vector<VkBuffer> buffers{};
		for (const auto& vulkanBuffer : m_VertexBuffers) {
			if (vulkanBuffer != nullptr) {
				buffers.push_back(vulkanBuffer->getBuffer());
			}
		}

		vkCmdBindVertexBuffers(commandBuffer.getCmdBuffer(), 0, 1, 
			buffers.data(), offsets);

		if (!m_DescriptorSets.empty()) {
			vkCmdBindDescriptorSets(commandBuffer.getCmdBuffer(),
				VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
				0, 1, m_DescriptorSets.data(), 
				0, nullptr);
		}

		if (m_UseIndexing) {
			if (m_IndexBuffer == nullptr) {
				Z_LOG_TXT("VulkanDrawer::onNewFrame", "warning: pIndexBuffer is nullptr");
				return;
			}

			const std::shared_ptr<VulkanBuffer> _indexBuffer =
				std::dynamic_pointer_cast<VulkanBuffer>(m_IndexBuffer);

			vkCmdBindIndexBuffer(commandBuffer.getCmdBuffer(),
				_indexBuffer->getBuffer(), 0,
				VkIndexType::VK_INDEX_TYPE_UINT16);

			vkCmdDrawIndexed(commandBuffer.getCmdBuffer(), m_IndexCount, m_InstanceCount, m_FirstIndex,
				m_VertexOffset, m_FirstInstance);
		}
		else {
			vkCmdDraw(commandBuffer.getCmdBuffer(), m_VertexCount, m_InstanceCount,
				m_FirstVertex, m_FirstInstance);
		}
	}

}