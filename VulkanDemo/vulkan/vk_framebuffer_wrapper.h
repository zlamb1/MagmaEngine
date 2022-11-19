#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "vk_obj.h"
#include "vk_swap_chain_wrapper.h"
#include "vk_render_pass_wrapper.h"

struct _VkFramebuffer : VulkanWrapper {

	VkSwapChainWrapper* pSwapchainWrapper;
	VkDevice* pDevice;
	VkRenderPass* pRenderPass; 

	std::vector<VkFramebuffer> vkFramebuffers{};

	_VkFramebuffer() {}
	~_VkFramebuffer();

	VkResult create();

};