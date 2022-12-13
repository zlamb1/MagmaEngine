#pragma once

#include "Magma/Vulkan/Command/vulkan_cmd.h"

#include "Magma/Vulkan/Image/image_view.h"

#include "Magma/Vulkan/Render/vulkan_renderer.h"
#include "Magma/Vulkan/Render/vulkan_framebuffer.h"

#include "Magma/Vulkan/Shader/shader_attributes.h"

namespace Magma {

	class VulkanPipeline : public VulkanObject {

	public:
		VulkanPipeline(VulkanShaderAttributes& pShaderAttributes);
		~VulkanPipeline() override;

		VkResult init() override;
		VkResult initFramebuffers();

		void addShader(std::shared_ptr<MagmaShader> pVulkanShader);

		void destroyFramebuffers();
		void destroyPipeline();

		void onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer, uint32_t imageIndex);

	public:
		std::shared_ptr<Swapchain> pVulkanSwapchain = nullptr;
		std::shared_ptr<VulkanRenderer> pVulkanRenderer = nullptr;
		std::shared_ptr<VulkanImageView> pDepthImageView = nullptr;

		VulkanShaderAttributes& pShaderAttributes;

	private:
		VulkanShaderPipeline vulkanShaderPipeline{};
		std::shared_ptr<VulkanFixedFunctionState> vulkanFixedFunctionState = nullptr;
		std::shared_ptr<VulkanRenderPass> vulkanRenderPass = nullptr;
		std::shared_ptr<VulkanFramebuffer> vulkanFramebuffer = nullptr;

		VkPipeline vkPipeline{};

	};

}