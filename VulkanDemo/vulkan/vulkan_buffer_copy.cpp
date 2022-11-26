#include "vulkan_buffer_copy.h"

VkResult VulkanBufferCopy::init() {
	VulkanOperation::init();

	vulkanCmdBuffer->record();
	VkBufferCopy vkBufferCopy{};
	vkBufferCopy.srcOffset = 0; // Optional
	vkBufferCopy.dstOffset = 0; // Optional
	vkBufferCopy.size = pSize;
	vkCmdCopyBuffer(vulkanCmdBuffer->getCmdBuffer(), pSrc, pDst, 1, &vkBufferCopy);
	vulkanCmdBuffer->end();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vulkanCmdBuffer->getCmdBuffer();

	vkQueueSubmit(pDevice->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	// VkFence will allow for scheduling multiple transfers simultaneously
	vkQueueWaitIdle(pDevice->getGraphicsQueue());

	return VK_SUCCESS;
}