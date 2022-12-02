#pragma once

#include <iostream>
#include <stdexcept>

#include "Window/vulkan_impl.h"

#include "Vulkan/Logging/vulkan_logger.h"

#include "Vulkan/Setup/vulkan_instance.h"

namespace Magma {
	class VulkanSurface : public VulkanObject {

	public:
		VulkanSurface(VulkanImpl& pWindowImpl,
			std::shared_ptr<VulkanInstance> pVulkanInstance);
		~VulkanSurface() override;

		VkResult init() override;

		VkSurfaceKHR& getSurfaceKHR();

	public:
		VulkanImpl& pWindowImpl;
		std::shared_ptr<VulkanInstance> pVulkanInstance;

	private:
		VkSurfaceKHR vkSurfaceKHR{};

	};
}