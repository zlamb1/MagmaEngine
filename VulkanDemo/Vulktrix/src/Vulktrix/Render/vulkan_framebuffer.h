#pragma once

#include "Vulktrix/Pipeline/vulkan_pipeline_c.h"

#include "Vulktrix/Surface/vulkan_swapchain.h"

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