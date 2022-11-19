#pragma once

#include <vulkan/vulkan.h>

struct VulkanWrapper {

	VkAllocationCallbacks* pAllocator = nullptr;
	virtual VkResult create() = 0;

};