#include "vulkan_operation.h"

namespace Magma {

	VulkanOperation::VulkanOperation(std::shared_ptr<VulkanDevice> pVulkanDevice) :
		pVulkanDevice{ pVulkanDevice }, pVulkanCmdPool{ std::make_shared<VulkanCmdPool>(pVulkanDevice) } {}

	VkResult VulkanOperation::init() {
		pVulkanCmdPool->pFlag = VulkanCommandPoolCreateFlag::TRANSIENT;
		pVulkanCmdPool->init();

		vulkanCmdBuffer = std::make_shared<VulkanCmdBuffer>(pVulkanDevice, pVulkanCmdPool);
		vulkanCmdBuffer->init();

		return VK_SUCCESS;
	}

}