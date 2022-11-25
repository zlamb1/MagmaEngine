#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_object.h"
#include "vulkan_swapchain.h"
#include "vulkan_logger.h"

class VulkanFramebuffer : VulkanObject {

public:
	VulkanFramebuffer() = default;
	~VulkanFramebuffer() override;

	VulkanSwapchain* pSwapchain;
	VkDevice* pDevice;
	VkRenderPass* pRenderPass; 

	VkResult init() override;
	
	std::vector<VkFramebuffer>& getFramebuffers();

private:
	std::vector<VkFramebuffer> vkFramebuffers{};

};