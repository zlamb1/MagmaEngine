#include "vulkan_operation.h"

VulkanOperation::~VulkanOperation() {
	if (vulkanCmdBuffer != nullptr) {
		delete vulkanCmdBuffer; 
	}
	if (!cmdPoolSet && pCmdPool != nullptr) {
		delete pCmdPool;
	}
}

VkResult VulkanOperation::init() {
	if (pDevice == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanOperation::init", "pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pCmdPool == nullptr) {
		pCmdPool = new VulkanCmdPool();
		pCmdPool->pDevice = &pDevice->getDevice();
		pCmdPool->pFlag = VulkanCommandPoolCreateFlag::TRANSIENT;
		pCmdPool->pQueueFamily = &pDevice->getQueueFamily();
		pCmdPool->init();
		cmdPoolSet = false;
	}

	if (vulkanCmdBuffer != nullptr) {
		delete vulkanCmdBuffer;
	}

	vulkanCmdBuffer = new VulkanCmdBuffer();
	vulkanCmdBuffer->pDevice = &pDevice->getDevice();
	vulkanCmdBuffer->pCmdPool = &pCmdPool->getCmdPool();
	vulkanCmdBuffer->init();
	
	return VK_SUCCESS;
}

void VulkanOperation::setCmdPool(VulkanCmdPool* pCmdPool) {
	if (this->pCmdPool != nullptr) {
		delete this->pCmdPool;
	}
	this->pCmdPool = pCmdPool;
	cmdPoolSet = true; 
}