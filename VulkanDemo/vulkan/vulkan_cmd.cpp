#include "vulkan_cmd.h"

// VulkanCmdPool

VulkanCmdPool::~VulkanCmdPool() {
	if (pDevice != nullptr) {
		vkDestroyCommandPool(*pDevice, vkCmdPool, nullptr);
	}
}

VkResult VulkanCmdPool::init() {
	if (pDevice == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanCmdPool::init", "pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pQueueFamily == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanCmdPool::init", "pQueueFamily is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkCommandPoolCreateInfo poolInfo{};

	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = (VkCommandPoolCreateFlags) pFlag;
	poolInfo.queueFamilyIndex = pQueueFamily->getGraphics();

	return vkCreateCommandPool(*pDevice, &poolInfo, pAllocator, &vkCmdPool);
}

VkCommandPool& VulkanCmdPool::getCmdPool() {
	return vkCmdPool;
}

// VulkanCmdBuffer

VulkanCmdBuffer::~VulkanCmdBuffer() {
	if (pDevice != nullptr && pCmdPool != nullptr) {
		vkFreeCommandBuffers(*pDevice, *pCmdPool, 1, &vkCmdBuffer);
	}
}

VkResult VulkanCmdBuffer::init() {
	if (pDevice == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanCmdBuffer::init", "pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pCmdPool == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanCmdBuffer::init", "pCmdPool is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkCommandBufferAllocateInfo cmdBufferAllocInfo{};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.commandPool = *pCmdPool;
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferAllocInfo.commandBufferCount = 1;

	return vkAllocateCommandBuffers(*pDevice, &cmdBufferAllocInfo, &vkCmdBuffer);
}

VkResult VulkanCmdBuffer::record() {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // optional
	beginInfo.pInheritanceInfo = nullptr; // optional

	return vkBeginCommandBuffer(vkCmdBuffer, &beginInfo);
}

void VulkanCmdBuffer::end() {
	vkEndCommandBuffer(vkCmdBuffer);
}

void VulkanCmdBuffer::reset() {
	vkResetCommandBuffer(vkCmdBuffer, 0);
}

VkCommandBuffer& VulkanCmdBuffer::getCmdBuffer() {
	return vkCmdBuffer;
}