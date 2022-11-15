#pragma once

#include <vulkan/vulkan.h>

#include "vk_device_wrapper.h"
#include "vk_graphics_pipeline.h"
#include "vk_fixed_function_wrapper.h"
#include "vk_render_pass_wrapper.h"
#include "vk_framebuffer_wrapper.h"
#include "vk_command_wrapper.h"

class VkPipelineWrapper {

	public:

		VkPipelineWrapper(VkDeviceWrapper& _vkDeviceWrapper, 
			VkSwapChainWrapper& _vkSwapChainWrapper);

		void newFrame(VkCommandWrapper& vkCommandWrapper, uint32_t imageIndex);

		void init();

	private:

		VkDeviceWrapper& vkDeviceWrapper;
		VkSwapChainWrapper& vkSwapChainWrapper;

		std::unique_ptr<VkGraphicsPipeline> vkGraphicsPipeline;
		std::unique_ptr<VkFixedFunctionWrapper> vkFixedFunctionWrapper;
		std::unique_ptr<VkRenderPassWrapper> vkRenderPassWrapper;
		std::unique_ptr<VkFramebufferWrapper> vkFramebufferWrapper;

		VkPipeline vkPipeline;

		void initGraphicsPipeline();
		void initFixedFunctionState();
		void initRenderPass();
		void initFramebuffers();

};