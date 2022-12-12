#include "vulkan_image.h"

namespace Magma {

	VulkanImage::VulkanImage(std::shared_ptr<VulkanDevice> pVulkanDevice) :
		pVulkanDevice{ pVulkanDevice }, memoryAllocator{ pVulkanDevice->getMemoryAllocator() } {}

	VulkanImage::~VulkanImage() {
		if (memoryAllocator != nullptr)
			vmaDestroyImage(memoryAllocator->getAllocator(), vkImage, vmaAlloc);
	}

	VkResult VulkanImage::init() {
		VkImageCreateInfo imageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageCreateInfo.imageType = pType;
		imageCreateInfo.extent.width = pExtentWidth;
		imageCreateInfo.extent.height = pExtentHeight;
		imageCreateInfo.extent.depth = pExtentDepth;
		imageCreateInfo.mipLevels = pMipLevels;
		imageCreateInfo.arrayLayers = pArrayLayers;
		imageCreateInfo.format = pFormat;
		imageCreateInfo.tiling = pTiling;
		imageCreateInfo.initialLayout = pInitialLayout;
		imageCreateInfo.usage = pUsageFlags;
		imageCreateInfo.sharingMode = pSharingMode;
		imageCreateInfo.samples = pSamples;
		imageCreateInfo.flags = pFlags;

		vmaImageAllocCreateInfo = {};
		vmaImageAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

		return vmaCreateImage(memoryAllocator->getAllocator(), &imageCreateInfo, &vmaImageAllocCreateInfo,
			&vkImage, &vmaAlloc, nullptr);
	}

	const VkImage& VulkanImage::getImage() const {
		return vkImage; 
	}

}