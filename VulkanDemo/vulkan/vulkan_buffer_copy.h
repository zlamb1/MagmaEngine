#pragma once

#include "vulkan_operation.h"

namespace BufferCopy {
	extern VkResult copyBuffer(VulkanDevice* pDevice, VkDeviceSize pSize, VkBuffer& pSrc, VkBuffer& pDst);
	extern VkResult copyBuffer(VulkanDevice* pDevice, VkDeviceSize pSize,
		VkBuffer& pSrc, VkBuffer& pDst, VkDeviceSize pSrcOffset);
	extern VkResult copyBuffer(VulkanDevice* pDevice, VkDeviceSize pSize,
		VkBuffer& pSrc, VkBuffer& pDst, VkDeviceSize pSrcOffset, VkDeviceSize pDstOffset);
}

class VulkanBufferCopy : public VulkanOperation {

public:
	VulkanBufferCopy(VulkanDevice& pDevice, VkBuffer& pSrc, VkBuffer& pDst);

	VkBuffer& pSrc;
	VkBuffer& pDst;

	VkDeviceSize pSize = 0, pSrcOffset = 0, pDstOffset = 0;

	VkResult init() override;

};