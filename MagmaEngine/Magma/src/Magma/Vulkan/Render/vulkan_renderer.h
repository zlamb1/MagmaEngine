#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "Magma/Render/renderer.h"

#include "Magma/Vulkan/Command/vulkan_cmd.h"
#include "Magma/Vulkan/Memory/vulkan_buffer.h"

namespace Magma {

	class VulkanRenderer : public Renderer {

	public:
		VulkanRenderer() = default;
		~VulkanRenderer() = default;

		void onNewFrame(VulkanCmdBuffer& commandBuffer, const VkPipelineLayout& pipelineLayout) const;
	
	public:
		std::vector<std::shared_ptr<VulkanBuffer>> m_VertexBuffers{};
		std::vector<VkDescriptorSet> m_DescriptorSets{};

	};

}