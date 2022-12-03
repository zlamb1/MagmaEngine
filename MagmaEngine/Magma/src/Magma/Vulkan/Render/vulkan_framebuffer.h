#pragma once

#include "Vulkan/Pipeline/vulkan_pipeline_c.h"

#include "Vulkan/Surface/vulkan_swapchain.h"

namespace Magma {

	class VulkanFramebuffer : VulkanObject {

	public:
		VulkanFramebuffer(std::shared_ptr<VulkanDevice> pVulkanDevice,
			std::shared_ptr<VulkanSwapchain> pVulkanSwapchain);
		~VulkanFramebuffer() override;

		VkResult init() override;

		std::vector<VkFramebuffer>& getFramebuffers();

		void destroyFramebuffers();
	
	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;
		std::shared_ptr<VulkanSwapchain> pVulkanSwapchain = nullptr;
		std::shared_ptr<VulkanRenderPass> pVulkanRenderPass = nullptr;

	private:
		std::vector<VkFramebuffer> vkFramebuffers{};

	};

}