#pragma once

#include <vulkan/vulkan.h>

#include "vk_device_wrapper.h"
#include "vk_graphics_pipeline.h"
#include "vk_fixed_function_wrapper.h"
#include "vk_render_pass_wrapper.h"
#include "vk_framebuffer_wrapper.h"
#include "vk_cmd_wrapper.h"

struct _VkPipeline : VulkanWrapper {

	_VkPipeline();
	~_VkPipeline();

	_VkLogger& _vkLogger; 

	_VkDevice* _pDevice = nullptr;
	_VkSwapchain* _pSwapchain = nullptr;

	std::unique_ptr<VkGraphicsPipeline> vkGraphicsPipeline;
	std::unique_ptr<VkFixedFunctionWrapper> vkFixedFunctionWrapper;
	std::unique_ptr<VkRenderPassWrapper> vkRenderPassWrapper;
	std::unique_ptr<_VkFramebuffer> vkFramebuffer;

	VkPipeline vkPipeline{};

	VkResult create();

	void onNewFrame(_VkCmdBuffer& vkCmdBuffer, uint32_t imageIndex);

};