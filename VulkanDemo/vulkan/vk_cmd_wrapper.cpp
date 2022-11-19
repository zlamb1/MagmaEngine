#include "vk_cmd_wrapper.h"

// VkCmdPoolWrapper Implementation

VkCmdPoolWrapper::~VkCmdPoolWrapper() {
	vkDestroyCommandPool(*pDevice, vkCmdPool, nullptr);
}

VkResult VkCmdPoolWrapper::create() {
	VkCommandPoolCreateInfo poolInfo{};

	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = pQueueFamily->getGraphics();

	return vkCreateCommandPool(*pDevice, &poolInfo, pAllocator, &vkCmdPool);
}

// VkCmdBufferWrapper Implementation

VkResult VkCmdBufferWrapper::create() {
	VkCommandBufferAllocateInfo allocInfo{};

	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = pCmdPool->vkCmdPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	return vkAllocateCommandBuffers(*pDevice, &allocInfo, &vkCmdBuffer);
}

VkResult VkCmdBufferWrapper::recordCmdBuffer() {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // optional
	beginInfo.pInheritanceInfo = nullptr; // optional

	return vkBeginCommandBuffer(vkCmdBuffer, &beginInfo);
}

void VkCmdBufferWrapper::resetCmdBuffer() {
	vkResetCommandBuffer(vkCmdBuffer, 0);
}