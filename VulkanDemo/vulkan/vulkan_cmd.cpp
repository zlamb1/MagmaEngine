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
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = pQueueFamily->getGraphics();

	return vkCreateCommandPool(*pDevice, &poolInfo, pAllocator, &vkCmdPool);
}

VkCommandPool& VulkanCmdPool::getCmdPool() {
	return vkCmdPool;
}

// VulkanCmdBuffer

VkResult VulkanCmdBuffer::init() {
	VkCommandBufferAllocateInfo allocInfo{};

	if (pDevice == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanCmdBuffer::init", "pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pCmdPool == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanCmdBuffer::init", "pCmdPool is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = *pCmdPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	return vkAllocateCommandBuffers(*pDevice, &allocInfo, &vkCmdBuffer);
}

VkResult VulkanCmdBuffer::record() {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // optional
	beginInfo.pInheritanceInfo = nullptr; // optional

	return vkBeginCommandBuffer(vkCmdBuffer, &beginInfo);
}

void VulkanCmdBuffer::reset() {
	vkResetCommandBuffer(vkCmdBuffer, 0);
}

VkCommandBuffer& VulkanCmdBuffer::getCmdBuffer() {
	return vkCmdBuffer;
}