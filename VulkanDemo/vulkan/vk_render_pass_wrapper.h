#pragma once

#include "vk_swap_chain_wrapper.h"

#include <vulkan/vulkan.h>

class VkRenderPassWrapper {

	public:

		VkRenderPassWrapper(VkDeviceWrapper& _vkDeviceWrapper,
			VkSwapChainWrapper& _vkSwapChainWrapper);
		~VkRenderPassWrapper();

		void init();

		VkRenderPass& getRenderPass();

	private:

		VkDeviceWrapper& vkDeviceWrapper;
		VkSwapChainWrapper& vkSwapChainWrapper;

		VkRenderPass vkRenderPass;
};