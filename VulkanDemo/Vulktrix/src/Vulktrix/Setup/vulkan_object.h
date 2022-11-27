#pragma once

#include <vulkan/vulkan.h>

class VulkanObject {

public:
	virtual ~VulkanObject() = default;

	VkAllocationCallbacks* pAllocator = nullptr;

	virtual VkResult init() { return VK_SUCCESS; };

};