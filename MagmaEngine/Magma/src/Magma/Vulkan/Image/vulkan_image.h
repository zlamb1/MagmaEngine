#pragma once

#include "Magma/Vulkan/Device/vulkan_device.h"
#include "Magma/Vulkan/Shader/shader_attributes.h"

namespace Magma {

	class VulkanImage : public VulkanObject {

		public:
			VulkanImage(std::shared_ptr<VulkanDevice> pVulkanDevice);
			~VulkanImage();

			VkResult init();

			const VkImage& getImage() const; 

		public:
			std::shared_ptr<VulkanDevice> pVulkanDevice;
			uint32_t pExtentWidth = 0, pExtentHeight = 0, pExtentDepth = 1; 
			uint32_t pMipLevels = 1, pArrayLevels = 1;

			VkImageType pImageType = VK_IMAGE_TYPE_2D;
			VkFormat pFormat;
			VkImageTiling pImageTiling = VK_IMAGE_TILING_OPTIMAL;
			VkImageLayout pImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkImageUsageFlags pUsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			VkSharingMode pSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			VkSampleCountFlagBits pSampleCountFlags = VK_SAMPLE_COUNT_1_BIT;
			VkImageCreateFlags pFlags = 0; 

		private:
			const MemoryAllocator* const memoryAllocator;
			VmaAllocationCreateInfo vmaImageAllocCreateInfo;
			VmaAllocation vmaAlloc;

			VkImage vkImage; 

	};

}