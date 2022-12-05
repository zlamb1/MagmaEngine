#pragma once

#include "Magma/Vulkan/Command/vulkan_operation.h"

namespace Magma {

	namespace BufferCopy {
		extern VkResult copyBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice, VkDeviceSize pSize,
			VkBuffer& pSrc, VkBuffer& pDst);
		extern VkResult copyBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice, VkDeviceSize pSize,
			VkBuffer& pSrc, VkBuffer& pDst, VkDeviceSize pSrcOffset);
		extern VkResult copyBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice, VkDeviceSize pSize,
			VkBuffer& pSrc, VkBuffer& pDst, VkDeviceSize pSrcOffset, VkDeviceSize pDstOffset);
	}


	class VulkanBufferCopy : public VulkanOperation {

	public:
		VulkanBufferCopy(std::shared_ptr<VulkanDevice> pVulkanDevice, VkBuffer& pSrc, VkBuffer& pDst);

		VkBuffer& pSrc;
		VkBuffer& pDst;

		VkDeviceSize pSize = 0, pSrcOffset = 0, pDstOffset = 0;

		VkResult init() override;

	};

}