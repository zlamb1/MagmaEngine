#pragma once

#include <vulkan/vulkan.h>

#include "window.h"

namespace Window {
	class VulkanImpl : public WindowImpl {
		public:
			virtual VkResult getSurfaceKHR(VkInstance& vkInstance,
				VkSurfaceKHR& vkSurfaceKHR) {
				return VK_SUCCESS;
			}
	};
}