#pragma once

#include <vulkan/vulkan.h>

namespace Magma {
	class VulkanObject {

	public:
		virtual ~VulkanObject() = default;
		virtual VkResult init() { return VK_SUCCESS; };

	public:
		VkAllocationCallbacks* pAllocator = nullptr;

	};
}