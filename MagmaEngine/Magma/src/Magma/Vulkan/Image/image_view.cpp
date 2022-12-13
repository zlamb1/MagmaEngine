#include "image_view.h"

namespace Magma {

	VulkanImageView::VulkanImageView(std::shared_ptr<VulkanDevice> pVulkanDevice,
		std::shared_ptr<VulkanImage> pVulkanImage) : pVulkanDevice{ pVulkanDevice },
		pVulkanImage{ pVulkanImage } {}

	VulkanImageView::~VulkanImageView() {
		if (pVulkanDevice != nullptr) {
			vkDestroyImageView(pVulkanDevice->getDevice(), vkImageView, pAllocator);
		}
	}

	VkResult VulkanImageView::init() {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = pVulkanImage->getImage();
		viewInfo.viewType = pImageViewType;
		viewInfo.format = getImageFormat();
		viewInfo.subresourceRange.aspectMask = pImageAspectFlags;
		viewInfo.subresourceRange.baseMipLevel = pBaseMipLevel;
		viewInfo.subresourceRange.levelCount = pLevelCount;
		viewInfo.subresourceRange.baseArrayLayer = pBaseArrayLayer;
		viewInfo.subresourceRange.layerCount = pLayerCount;

		auto createImageViewResult = vkCreateImageView(pVulkanDevice->getDevice(), &viewInfo,
			nullptr, &vkImageView);
		Z_LOG_OBJ("VulkanImageView::init::vkCreateImageView", createImageViewResult);

		return createImageViewResult;
	}

	const VkImageView& VulkanImageView::getImageView() const {
		return vkImageView; 
	}

	VkFormat VulkanImageView::getImageFormat() const {
		return static_cast<VkFormat>(pVulkanImage->m_Format);
	}

}