#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "vk_obj.h"
#include "vk_swap_chain_wrapper.h"
#include "vk_render_pass_wrapper.h"

struct _VkFramebuffer : VulkanWrapper {

	_VkFramebuffer() {}
	~_VkFramebuffer();

	_VkSwapchain* _pSwapchain;
	VkDevice* pDevice;
	VkRenderPass* pRenderPass; 

	std::vector<VkFramebuffer> vkFramebuffers{};

	VkResult create();

};