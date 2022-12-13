#pragma once

#include "Magma/Vulkan/Image/image_view.h"
#include "Magma/Vulkan/Pipeline/vulkan_pipeline_c.h"
#include "Magma/Vulkan/Surface/swapchain.h"

namespace Magma {

	class VulkanFramebuffer : VulkanObject {

	public:
		VulkanFramebuffer(std::shared_ptr<VulkanDevice> pVulkanDevice,
			std::shared_ptr<Swapchain> pVulkanSwapchain);
		~VulkanFramebuffer() override;

		VkResult init() override;

		std::vector<VkFramebuffer>& getFramebuffers();

		void destroyFramebuffers();
	
	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;
		std::shared_ptr<Swapchain> pVulkanSwapchain = nullptr;
		std::shared_ptr<VulkanRenderPass> pVulkanRenderPass = nullptr;
		std::shared_ptr<VulkanImageView> pDepthImageView; 

	private:
		std::vector<VkFramebuffer> vkFramebuffers{};

	};

}