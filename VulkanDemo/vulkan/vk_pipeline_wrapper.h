#pragma once

#include <vulkan/vulkan.h>

#include "vk_device_wrapper.h"
#include "vk_graphics_pipeline.h"
#include "vk_fixed_function_wrapper.h"
#include "vk_render_pass_wrapper.h"
#include "vk_framebuffer_wrapper.h"
#include "vk_cmd_wrapper.h"

class VkPipelineWrapper {

public:
	VkPipelineWrapper(_VkDevice& _vkDevice,
		_VkSwapchain& _vkSwapchain);
	~VkPipelineWrapper();

	void init();

	void newFrame(_VkCmdBuffer& vkCmdBuffer, uint32_t imageIndex);

private:

	_VkLogger& _vkLogger;

	_VkDevice& _vkDevice;
	_VkSwapchain& _vkSwapchain;

	// store as unique pointers as destruction order doesn't matter
	std::unique_ptr<VkGraphicsPipeline> vkGraphicsPipeline;
	std::unique_ptr<VkFixedFunctionWrapper> vkFixedFunctionWrapper;
	std::unique_ptr<VkRenderPassWrapper> vkRenderPassWrapper;
	std::unique_ptr<_VkFramebuffer> vkFramebuffer;

	VkPipeline vkPipeline;

	void initGraphicsPipeline();
	void initFixedFunctionState();
	void initRenderPass();
	void initFramebuffers();
		
};