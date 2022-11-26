#pragma once

#include "vulkan_operation.h"

namespace BufferCopy {
	extern VkResult copyBuffer(VulkanDevice* pDevice, VkDeviceSize pSize, VkBuffer pSrc, VkBuffer pDst);
	extern VkResult copyBuffer(VulkanDevice* pDevice, VkDeviceSize pSize,
		VkBuffer pSrc, VkBuffer pDst, VkDeviceSize pSrcOffset);
	extern VkResult copyBuffer(VulkanDevice* pDevice, VkDeviceSize pSize,
		VkBuffer pSrc, VkBuffer pDst, VkDeviceSize pSrcOffset, VkDeviceSize pDstOffset);
}

class VulkanBufferCopy : public VulkanOperation {

public:
	VulkanBufferCopy() = default;

	VkBuffer pSrc{};
	VkBuffer pDst{};
	VkDeviceSize pSize{};

	VkDeviceSize pSrcOffset{};
	VkDeviceSize pDstOffset{};

	VkResult init() override;

};