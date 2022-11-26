#include "vulkan_operation.h"

VulkanOperation::VulkanOperation(VulkanDevice& pDevice) : pDevice{ pDevice } {
	
}

VkResult VulkanOperation::init() {
	pCmdPool.pDevice = &pDevice.getDevice();
	pCmdPool.pFlag = VulkanCommandPoolCreateFlag::TRANSIENT;
	pCmdPool.pQueueFamily = &pDevice.getQueueFamily();
	pCmdPool.init();

	vulkanCmdBuffer = VulkanCmdBuffer();
	vulkanCmdBuffer.pDevice = &pDevice.getDevice();
	vulkanCmdBuffer.pCmdPool = &pCmdPool.getCmdPool();
	vulkanCmdBuffer.init();
	
	return VK_SUCCESS;
}