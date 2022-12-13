#include "vulkan_image.h"

#include "Magma/Vulkan/Render/render_enums.h"

namespace Magma {

	VulkanImage::VulkanImage(std::shared_ptr<VulkanDevice> pVulkanDevice) :
		pVulkanDevice{ pVulkanDevice }, memoryAllocator{ pVulkanDevice->getMemoryAllocator() } {}

	VulkanImage::~VulkanImage() {
		if (memoryAllocator != nullptr)
			vmaDestroyImage(memoryAllocator->getAllocator(), vkImage, vmaAlloc);
	}

	VkResult VulkanImage::init() {
		VkImageCreateInfo imageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageCreateInfo.imageType = convertImageType(m_Type);
		imageCreateInfo.extent.width = m_ExtentWidth;
		imageCreateInfo.extent.height = m_ExtentHeight;
		imageCreateInfo.extent.depth = m_ExtentDepth;
		imageCreateInfo.mipLevels = m_MipLevels;
		imageCreateInfo.arrayLayers = m_ArrayLayers;
		imageCreateInfo.format = static_cast<VkFormat>(m_Format);
		imageCreateInfo.tiling = convertImageTiling(m_Tiling);
		imageCreateInfo.initialLayout = convertImageLayout((m_InitialLayout));
		imageCreateInfo.usage = m_ImageUsage;
		imageCreateInfo.sharingMode = convertSharingMode(m_SharingMode);
		imageCreateInfo.samples = convertSampleCount(m_Samples);
		imageCreateInfo.flags = m_Flags;

		vmaImageAllocCreateInfo = {};
		vmaImageAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

		return vmaCreateImage(memoryAllocator->getAllocator(), &imageCreateInfo, &vmaImageAllocCreateInfo,
			&vkImage, &vmaAlloc, nullptr);
	}

	const VkImage& VulkanImage::getImage() const {
		return vkImage; 
	}

}
