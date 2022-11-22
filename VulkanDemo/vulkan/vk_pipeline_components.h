#pragma once

#include <fstream>
#include <iostream>

#include <shaderc/shaderc.hpp>
#include <shaderc/shaderc.h>
#include <vulkan/vulkan.h>

#include "vk_device_wrapper.h"
#include "vk_swapchain.h"
#include "vk_shader.h"

enum _ShaderType {
	VERTEX = shaderc_shader_kind::shaderc_vertex_shader,
	FRAGMENT = shaderc_shader_kind::shaderc_fragment_shader,
	COMPUTE = shaderc_shader_kind::shaderc_compute_shader
};

struct _VkShaderInfo {
	const char* pCode; 
	_ShaderType pShaderType;
};

struct _VkShaderPipeline : VulkanWrapper  {

	_VkShaderPipeline();
	~_VkShaderPipeline();

	_VkDevice* _pDevice = nullptr;

	std::vector<_VkShader*> _vkShaderHandles{};
	
	VkResult create();

	void addShader(_VkShader* _vkShaderHandle);

	std::vector<VkPipelineShaderStageCreateInfo> getShaderStages();

};

struct _VkFixedFunctionState : VulkanWrapper {
	
	_VkFixedFunctionState();
	~_VkFixedFunctionState();

	_VkDevice* _pDevice = nullptr;
	_VkSwapchain* _pSwapchain = nullptr;

	std::vector<VkVertexInputBindingDescription> pVertexBindingDescriptions{};
	std::vector<VkVertexInputAttributeDescription> pVertexAttributeDescriptions{};

	VkPipelineDynamicStateCreateInfo vkDynamicState{};
	std::vector<VkDynamicState> vkDynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineVertexInputStateCreateInfo vkVertexInputState{};
	VkPipelineInputAssemblyStateCreateInfo vkInputAssemblyState{};
	VkViewport vkViewport{};
	VkRect2D vkScissor{};
	VkPipelineViewportStateCreateInfo vkViewportState{};
	VkPipelineRasterizationStateCreateInfo vkRasterizerState{};
	VkPipelineMultisampleStateCreateInfo vkMultisamplingState{};
	VkPipelineDepthStencilStateCreateInfo vkDepthStencilState{};
	VkPipelineColorBlendAttachmentState vkColorBlendAttachmentState{};
	VkPipelineColorBlendStateCreateInfo vkColorBlendingState{};
	VkPipelineLayoutCreateInfo vkPipelineLayoutInfo{};

	VkPipelineLayout vkPipelineLayout{};

	VkResult create();

};

struct _VkRenderPass : VulkanWrapper {

	_VkRenderPass();
	~_VkRenderPass();

	_VkDevice* _pDevice = nullptr;
	_VkSwapchain* _pSwapchain = nullptr;

	VkRenderPass vkRenderPass{};

	VkResult create();

};