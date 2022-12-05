#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_image.h"

namespace Magma {

	class VulkanImageView : public VulkanObject {

	public:
		VulkanImageView(std::shared_ptr<VulkanDevice> pVulkanDevice, 
			std::shared_ptr<VulkanImage> pVulkanImage);
		~VulkanImageView() override;

		VkResult init() override;
	
		const VkImageView& getImageView() const;
		const VkFormat getImageFormat() const; 

	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr; 
		std::shared_ptr<VulkanImage> pVulkanImage = nullptr;

		VkImageViewType pImageViewType = VK_IMAGE_VIEW_TYPE_2D;
		VkImageAspectFlagBits pImageAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT; 
		uint32_t pBaseMipLevel = 0, pLevelCount = 1, pBaseArrayLayer = 0, pLayerCount = 1;

	private:
		VkImageView vkImageView; 

	};

}