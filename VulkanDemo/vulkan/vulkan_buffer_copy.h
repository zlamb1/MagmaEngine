#pragma once

#include "vulkan_operation.h"

class VulkanBufferCopy : public VulkanOperation {

public:
	VulkanBufferCopy() = default;

	VkBuffer pSrc{};
	VkBuffer pDst{};
	VkDeviceSize pSize{};

	VkResult init() override;

};