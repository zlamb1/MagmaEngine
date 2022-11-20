#pragma once

#include <vulkan/vulkan.h>

#include "vk_device_wrapper.h"
#include "vk_pipeline_components.h"
#include "vk_framebuffer_wrapper.h"
#include "vk_cmd_wrapper.h"

struct _VkPipeline : VulkanWrapper {

	_VkPipeline();
	~_VkPipeline();

	_VkLogger& _vkLogger; 

	_VkDevice* _pDevice = nullptr;
	_VkSwapchain* _pSwapchain = nullptr;

	std::unique_ptr<_VkShaderPipeline> _vkShaderPipeline;
	std::unique_ptr<_VkFixedFunctionState> _vkFixedFunctionState;
	std::unique_ptr<_VkRenderPass> _vkRenderPass;
	std::unique_ptr<_VkFramebuffer> _vkFramebuffer;

	VkPipeline vkPipeline{};

	VkResult create();

	void onNewFrame(_VkCmdBuffer& vkCmdBuffer, uint32_t imageIndex);

};