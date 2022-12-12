#pragma once

#include <vulkan/vulkan.h>

namespace Magma {

	class Image {

	public:
		virtual VkResult init() = 0; 

	public:
		uint32_t pExtentWidth = 0, pExtentHeight = 0, pExtentDepth = 1;
		uint32_t pMipLevels = 1, pArrayLayers = 1;

		VkImageType pType = VK_IMAGE_TYPE_2D;
		VkFormat pFormat;
		VkImageTiling pTiling = VK_IMAGE_TILING_OPTIMAL;
		VkImageLayout pInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageUsageFlags pUsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkSharingMode pSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VkSampleCountFlagBits pSamples = VK_SAMPLE_COUNT_1_BIT;
		VkImageCreateFlags pFlags = 0;

	};

}