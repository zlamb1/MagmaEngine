#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "vk_swap_chain_wrapper.h"
#include "vk_render_pass_wrapper.h"

class VkFramebufferWrapper {

	public:

		VkFramebufferWrapper(VkDeviceWrapper& _vkDeviceWrapper,
			VkSwapChainWrapper& _vkSwapChainWrapper,
			VkRenderPassWrapper& _vkRenderPassWrapper);
		~VkFramebufferWrapper();

		std::vector<VkFramebuffer>& getFramebuffers();

		void init();

	private:
		
		VkDeviceWrapper& vkDeviceWrapper;
		VkSwapChainWrapper& vkSwapChainWrapper;
		VkRenderPassWrapper& vkRenderPassWrapper;

		std::vector<VkFramebuffer> vkFramebuffers;

};