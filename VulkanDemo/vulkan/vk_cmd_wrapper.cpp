#include "vk_cmd_wrapper.h"

// VkCmdPoolWrapper Implementation

_VkCmdPool::~_VkCmdPool() {
	vkDestroyCommandPool(*pDevice, vkCmdPool, nullptr);
}

VkResult _VkCmdPool::create() {
	auto logger = _VkLogger::Instance();

	if (pDevice == nullptr) {
		logger.LogText("_VkCmdPool => pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pQueueFamily == nullptr) {
		logger.LogText("_VkCmdPool => pQueueFamily is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkCommandPoolCreateInfo poolInfo{};

	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = pQueueFamily->getGraphics();

	return vkCreateCommandPool(*pDevice, &poolInfo, pAllocator, &vkCmdPool);
}

// VkCmdBufferWrapper Implementation

VkResult _VkCmdBuffer::create() {
	VkCommandBufferAllocateInfo allocInfo{};

	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = pCmdPool->vkCmdPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	return vkAllocateCommandBuffers(*pDevice, &allocInfo, &vkCmdBuffer);
}

VkResult _VkCmdBuffer::recordCmdBuffer() {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // optional
	beginInfo.pInheritanceInfo = nullptr; // optional

	return vkBeginCommandBuffer(vkCmdBuffer, &beginInfo);
}

void _VkCmdBuffer::resetCmdBuffer() {
	vkResetCommandBuffer(vkCmdBuffer, 0);
}