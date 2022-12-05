#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "Magma/Render/renderer.h"

#include "Magma/Vulkan/Command/vulkan_cmd.h"
#include "Magma/Vulkan/Memory/vulkan_buffer.h"
#include "Magma/Vulkan/Setup/vulkan_object.h"

namespace Magma {

	class VulkanRenderer : public Renderer {

	public:
		VulkanRenderer() = default;
		~VulkanRenderer() = default;

		void onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer, VkPipelineLayout& vkPipelineLayout);
	
	public:
		std::vector<std::shared_ptr<VulkanBuffer>> pVertexBuffers{};

		std::vector<VkDescriptorSet> pDescriptorSets{};

	};

}