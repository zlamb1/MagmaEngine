#pragma once

#include "vk_swap_chain_wrapper.h"

#include <vulkan/vulkan.h>

class VkRenderPassWrapper {

	public:

		VkRenderPassWrapper(_VkDevice& _vkDevice,
			_VkSwapchain& _vkSwapchain);
		~VkRenderPassWrapper();

		void init();

		VkRenderPass& getRenderPass();

	private:

		_VkDevice& _vkDevice;
		_VkSwapchain& _vkSwapchain;

		VkRenderPass vkRenderPass;
};