#pragma once

#include <vulkan/vulkan.h>

#include "Magma/Vulkan/Setup/vulkan_debugger.h"
#include "Magma/Vulkan/Setup/vulkan_validater.h"

namespace Magma {

	class VulkanInstance : public VulkanObject {

	public:
		VulkanInstance(std::shared_ptr<VulkanValidater> pVulkanValidater);
		~VulkanInstance() override;

		std::vector<const char*> pRequiredExtensions{};

		VkResult init() override;

		VkInstance& getInstance();

	public:
		VkDebugUtilsMessengerCreateInfoEXT pDebugCreateInfo{};
		std::shared_ptr<VulkanValidater> pVulkanValidater = nullptr;

	private:
		VkInstance vkInstance{};

	};

}

