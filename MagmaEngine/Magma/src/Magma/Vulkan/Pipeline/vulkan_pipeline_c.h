#pragma once

#include <fstream>
#include <iostream>

#include <shaderc/shaderc.hpp>
#include <shaderc/shaderc.h>

#include "Magma/Vulkan/Shader/shader_attributes.h"
#include "Magma/Vulkan/Shader/vulkan_shader.h"

#include "Magma/Vulkan/Surface/vulkan_swapchain.h"

namespace Magma {

	enum ShadercType {
		VERTEX = shaderc_shader_kind::shaderc_vertex_shader,
		FRAGMENT = shaderc_shader_kind::shaderc_fragment_shader,
		COMPUTE = shaderc_shader_kind::shaderc_compute_shader
	};

	class VulkanShaderInfo {

	public:
		const char* pCode;
		ShadercType pShaderType;

	};

	class VulkanShaderPipeline : public VulkanObject {

	public:
		VulkanShaderPipeline() = default;
		~VulkanShaderPipeline() override = default;

		VkResult init() override;

		std::vector<VkPipelineShaderStageCreateInfo> getShaderStages();

		void addShader(std::shared_ptr<MagmaShader> pVulkanShader);

	private:
		std::vector<std::shared_ptr<MagmaShader>> pVulkanShaders{};

	};

	class VulkanFixedFunctionState : public VulkanObject {

	public:
		VulkanFixedFunctionState(VulkanShaderAttributes& pShaderAttributes);
		~VulkanFixedFunctionState() override;

		VkResult init() override;

		VkPipelineDynamicStateCreateInfo& getDynamicCreateInfo();
		VkPipelineVertexInputStateCreateInfo& getVertexInputCreateInfo();
		VkPipelineInputAssemblyStateCreateInfo& getInputAssemblyCreateInfo();
		VkViewport& getViewport();
		VkRect2D& getScissor();
		VkPipelineViewportStateCreateInfo& getViewportCreateInfo();
		VkPipelineRasterizationStateCreateInfo& getRasterizationCreateInfo();
		VkPipelineMultisampleStateCreateInfo& getMultisampleCreateInfo();
		VkPipelineDepthStencilStateCreateInfo& getDepthStencilCreateInfo();
		VkPipelineColorBlendAttachmentState& getColorBlendAttachmentState();
		VkPipelineColorBlendStateCreateInfo& getColorBlendCreateInfo();
		VkPipelineLayoutCreateInfo& getPipelineLayoutCreateInfo();
		VkPipelineLayout& getPipelineLayout();

	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;
		std::shared_ptr<VulkanSwapchain> pVulkanSwapchain = nullptr;

		VulkanShaderAttributes& pShaderAttributes;

	private:
		VkPipelineDynamicStateCreateInfo vkDynamicCreateInfo{};

		std::vector<VkDynamicState> vkDynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineVertexInputStateCreateInfo vkVertexInputCreateInfo{};
		VkPipelineInputAssemblyStateCreateInfo vkInputAssemblyCreateInfo{};
		VkViewport vkViewport{};
		VkRect2D vkScissor{};
		VkPipelineViewportStateCreateInfo vkViewportCreateInfo{};
		VkPipelineRasterizationStateCreateInfo vkRasterizationCreateInfo{};
		VkPipelineMultisampleStateCreateInfo vkMultisampleCreateInfo{};
		VkPipelineDepthStencilStateCreateInfo vkDepthStencilCreateInfo{};
		VkPipelineColorBlendAttachmentState vkColorBlendAttachmentState{};
		VkPipelineColorBlendStateCreateInfo vkColorBlendCreateInfo{};
		VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo{};

		VkPipelineLayout vkPipelineLayout{};

		std::vector<VkVertexBinding> bindings{};
		std::vector<VkVertexAttribute> attributes{};

	};

	class VulkanRenderPass : public VulkanObject {

	public:
		VulkanRenderPass(std::shared_ptr<VulkanDevice> pVulkanDevice,
			std::shared_ptr<VulkanSwapchain> pVulkanSwapchain);
		~VulkanRenderPass() override;

		VkResult init() override;

		VkRenderPass& getRenderPass();
	
	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;
		std::shared_ptr<VulkanSwapchain> pVulkanSwapchain = nullptr;

		VkFormat pDepthFormat; 

	private:
		VkRenderPass vkRenderPass{};

	};

}
