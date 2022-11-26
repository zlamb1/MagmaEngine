#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_object.h"
#include "vulkan_swapchain.h"
#include "vulkan_logger.h"
#include "vulkan_pipeline_c.h"

class VulkanFramebuffer : VulkanObject {

public:
	VulkanFramebuffer(std::shared_ptr<VulkanDevice> pVulkanDevice, 
		std::shared_ptr<VulkanSwapchain> pVulkanSwapchain);
	~VulkanFramebuffer() override;

	std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;
	std::shared_ptr<VulkanSwapchain> pVulkanSwapchain = nullptr;
	std::shared_ptr<VulkanRenderPass> pVulkanRenderPass = nullptr; 

	VkResult init() override;

	std::vector<VkFramebuffer>& getFramebuffers();

	void destroyFramebuffers();

private:
	std::vector<VkFramebuffer> vkFramebuffers{};

};