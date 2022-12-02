#pragma once

#include "Vulkan/Command/vulkan_cmd.h"

#include "Vulkan/Render/vulkan_drawer.h"
#include "Vulkan/Render/vulkan_framebuffer.h"

#include "Vulkan/Shader/shader_attributes.h"

namespace Magma {

	class VulkanPipeline : public VulkanObject {

	public:
		VulkanPipeline(ShaderAttributes& pShaderAttributes);
		~VulkanPipeline() override;

		VkResult init() override;
		VkResult initFramebuffers();

		void addShader(std::shared_ptr<MagmaShader> pVulkanShader);

		void destroyFramebuffers();
		void destroyPipeline();

		void onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer, uint32_t imageIndex);

	public:
		std::shared_ptr<VulkanSwapchain> pVulkanSwapchain = nullptr;
		std::shared_ptr<VulkanDrawer> pVulkanDrawer = nullptr;

		ShaderAttributes& pShaderAttributes;

	private:
		VulkanShaderPipeline vulkanShaderPipeline{};
		std::shared_ptr<VulkanFixedFunctionState> vulkanFixedFunctionState = nullptr;
		std::shared_ptr<VulkanRenderPass> vulkanRenderPass = nullptr;
		std::shared_ptr<VulkanFramebuffer> vulkanFramebuffer = nullptr;

		VkPipeline vkPipeline{};

	};

}