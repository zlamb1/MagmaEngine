#pragma once

#include "vulkan_cmd.h"

#include "Magma/Vulkan/Image/vulkan_image.h"
#include "Magma/Vulkan/Memory/VMA/vma_buffer.h"

namespace Magma {

	class VulkanOperation {

	public:
		VulkanOperation(std::shared_ptr<VulkanDevice> pVulkanDevice);
		virtual ~VulkanOperation() = default;

		virtual VkResult init() = 0;

		void record();
		VkResult submit();

	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice;
		std::shared_ptr<VulkanCmdPool> pVulkanCmdPool;

	protected:
		std::shared_ptr<VulkanCmdBuffer> vulkanCmdBuffer;

	};
	
	class TransitionImage : public VulkanOperation {

	public:
		TransitionImage(std::shared_ptr<VulkanDevice> pVulkanDevice);
		VkResult init() override;

	public:
		std::shared_ptr<Image> pImage; VkFormat pFormat;
		VkImageLayout pOldLayout, pNewLayout;

	};

	class CopyBufferToImage : public VulkanOperation {

	public:
		CopyBufferToImage(std::shared_ptr<VulkanDevice> pVulkanDevice);
		VkResult init() override; 

	public:
		std::shared_ptr<Buffer> pBuffer; 
		std::shared_ptr<Image> pImage;

	};

}