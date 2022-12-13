#pragma once

#include "Magma/Window/vulkan_impl.h"

#include "Magma/Vulkan/Logging/vulkan_logger.h"

#include "Magma/Vulkan/Setup/instance.h"

namespace Magma {

	class Surface : public VulkanObject {

	public:
		Surface(VulkanImpl& pWindowImpl,
			std::shared_ptr<Instance> pVulkanInstance);
		~Surface() override;

		VkResult init() override;

		VkSurfaceKHR& getSurfaceKHR();

	public:
		VulkanImpl& m_WindowImpl;
		std::shared_ptr<Instance> m_Instance;

	private:
		VkSurfaceKHR m_SurfaceKHR{};

	};

}