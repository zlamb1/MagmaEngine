#pragma once

#include <fstream>
#include <iostream>

#include <shaderc/shaderc.hpp>
#include <shaderc/shaderc.h>
#include <vulkan/vulkan.h>

#include "vulkan_shader.h"

#include "Vulktrix/Memory/vulkan_descriptor.h"

#include "Vulktrix/Surface/vulkan_swapchain.h"

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

	void addShader(std::shared_ptr<VulkanShader> pVulkanShader);

private:
	std::vector<std::shared_ptr<VulkanShader>> pVulkanShaders{};

};

class VulkanFixedFunctionState : public VulkanObject {

public:
	VulkanFixedFunctionState(std::shared_ptr<VulkanDevice> pVulkanDevice,
		std::shared_ptr<VulkanSwapchain> pVulkanSwapchain);
	~VulkanFixedFunctionState() override;

	std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;
	std::shared_ptr<VulkanSwapchain> pVulkanSwapchain = nullptr;

	std::vector<VkVertexInputBindingDescription> pVertexBindingDescriptions{};
	std::vector<VkVertexInputAttributeDescription> pVertexAttributeDescriptions{};

	std::shared_ptr<VulkanDescriptorSetLayout> pVulkanDescriptorSetLayout = nullptr;

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

};

class VulkanRenderPass : public VulkanObject {

public:
	VulkanRenderPass(std::shared_ptr<VulkanDevice> pVulkanDevice,
		std::shared_ptr<VulkanSwapchain> pVulkanSwapchain);
	~VulkanRenderPass() override;

	std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;
	std::shared_ptr<VulkanSwapchain> pVulkanSwapchain = nullptr;

	VkResult init() override;

	VkRenderPass& getRenderPass();

private:
	VkRenderPass vkRenderPass{};

};