#pragma once

#include <vulkan/vulkan.h>

struct VulkanWrapper {

	virtual ~VulkanWrapper() {}

	VkAllocationCallbacks* pAllocator = nullptr;
	virtual VkResult create() = 0;

};