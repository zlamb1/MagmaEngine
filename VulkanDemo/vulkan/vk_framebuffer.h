#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "vk_obj.h"
#include "vk_swapchain.h"

struct _VkFramebuffer : VulkanWrapper {

	_VkFramebuffer();
	~_VkFramebuffer();

	_VkSwapchain* _pSwapchain;
	VkDevice* pDevice;
	VkRenderPass* pRenderPass; 

	std::vector<VkFramebuffer> vkFramebuffers{};

	VkResult create();

};