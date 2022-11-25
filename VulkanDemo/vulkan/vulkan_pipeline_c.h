#pragma once

#include <fstream>
#include <iostream>

#include <shaderc/shaderc.hpp>
#include <shaderc/shaderc.h>
#include <vulkan/vulkan.h>

#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_shader.h"

enum VulkanShaderType {
	VERTEX = shaderc_shader_kind::shaderc_vertex_shader,
	FRAGMENT = shaderc_shader_kind::shaderc_fragment_shader,
	COMPUTE = shaderc_shader_kind::shaderc_compute_shader
};

class VulkanShaderInfo {

public:
	const char* pCode; 
	VulkanShaderType pShaderType;

};

class VulkanShaderPipeline : public VulkanObject {

public:
	VulkanShaderPipeline() = default;
	~VulkanShaderPipeline() override = default;

	VulkanDevice* pDevice = nullptr;

	VkResult init() override;

	std::vector<VkPipelineShaderStageCreateInfo> getShaderStages();

	void addShader(VulkanShader* vulkanShaderHandle);

private:
	std::vector<VulkanShader*> vulkanShaderHandles{};

};

class VulkanFixedFunctionState : public VulkanObject {

public:
	VulkanFixedFunctionState() = default;
	~VulkanFixedFunctionState() override;

	VulkanDevice* pDevice = nullptr;
	VulkanSwapchain* pSwapchain = nullptr;
	std::vector<VkVertexInputBindingDescription> pVertexBindingDescriptions{};
	std::vector<VkVertexInputAttributeDescription> pVertexAttributeDescriptions{};

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
	VkPipelineDynamicStateCreateInfo vkDynamiCreateInfo{};

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
	VulkanRenderPass() = default;
	~VulkanRenderPass() override;

	VulkanDevice* pDevice = nullptr;
	VulkanSwapchain* pSwapchain = nullptr;

	VkResult init() override;

	VkRenderPass& getRenderPass();

private:
	VkRenderPass vkRenderPass{};

};