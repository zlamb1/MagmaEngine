#pragma once

#include "vulkan_cmd.h"

namespace Magma {

	class VulkanOperation {

	public:
		VulkanOperation(std::shared_ptr<VulkanDevice> pVulkanDevice);
		virtual ~VulkanOperation() = default;

		virtual VkResult init();

	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice;
		std::shared_ptr<VulkanCmdPool> pVulkanCmdPool;

	protected:
		std::shared_ptr<VulkanCmdBuffer> vulkanCmdBuffer;

	};

}