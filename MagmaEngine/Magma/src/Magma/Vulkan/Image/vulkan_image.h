#pragma once

#include "Magma/Image/image.h"

#include "Magma/Vulkan/Device/vulkan_device.h"

namespace Magma {

	class VulkanImage : public Image {

		public:
			VulkanImage(std::shared_ptr<VulkanDevice> pVulkanDevice);
			~VulkanImage();

			VkResult init() override;

			const VkImage& getImage() const; 

		public:
			std::shared_ptr<VulkanDevice> pVulkanDevice;

		private:
			const MemoryAllocator* const memoryAllocator;
			VmaAllocationCreateInfo vmaImageAllocCreateInfo;
			VmaAllocation vmaAlloc;

			VkImage vkImage; 

	};

}