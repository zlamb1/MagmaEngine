#include "vulkan_buffer_copy.h"

namespace BufferCopy {
	VkResult copyBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice, VkDeviceSize pSize, VkBuffer& pSrc, VkBuffer& pDst) {
		return copyBuffer(pVulkanDevice, pSize, pSrc, pDst, 0, 0);
	}

	VkResult copyBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice, VkDeviceSize pSize,
		VkBuffer& pSrc, VkBuffer& pDst, VkDeviceSize pSrcOffset) {
		return copyBuffer(pVulkanDevice, pSize, pSrc, pDst, pSrcOffset, 0);
	}

	VkResult copyBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice, VkDeviceSize pSize,
		VkBuffer& pSrc, VkBuffer& pDst, VkDeviceSize pSrcOffset, VkDeviceSize pDstOffset) {
		VulkanBufferCopy vulkanBufferCopy{ pVulkanDevice, pSrc, pDst };
		vulkanBufferCopy.pSize = pSize;
		vulkanBufferCopy.pSrcOffset = pSrcOffset;
		vulkanBufferCopy.pDstOffset = pDstOffset;
		return vulkanBufferCopy.init();
	}
}

VulkanBufferCopy::VulkanBufferCopy(std::shared_ptr<VulkanDevice> pVulkanDevice,
	VkBuffer& pSrc, VkBuffer& pDst) : VulkanOperation(pVulkanDevice), pSrc {
	pSrc }, pDst{ pDst } {}

VkResult VulkanBufferCopy::init() {
	auto initResult = VulkanOperation::init();
	if (initResult != VK_SUCCESS) {
		return initResult; 
	}

	vulkanCmdBuffer->record();
	VkBufferCopy bufferCopy{};
	bufferCopy.srcOffset = pSrcOffset;
	bufferCopy.dstOffset = pDstOffset;
	bufferCopy.size = pSize;
	vkCmdCopyBuffer(vulkanCmdBuffer->getCmdBuffer(), pSrc, pDst, 1, &bufferCopy);
	vulkanCmdBuffer->end();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vulkanCmdBuffer->getCmdBuffer();

	auto& deviceProfile = VulkanDeviceProfile::instance();
	auto graphicsOptional = deviceProfile.getQueue(VulkanQueueType::GRAPHICS);

	if (!graphicsOptional.has_value()) {
		VulkanLogger::instance().enqueueText("VulkanBufferCopy::init", "could not find graphics queue");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	auto graphicsQueue = graphicsOptional.value();

	auto queueSubmitResult = vkQueueSubmit(graphicsQueue, 1,
		&submitInfo, VK_NULL_HANDLE);
	if (queueSubmitResult != VK_SUCCESS) {
		return queueSubmitResult;
	}

	// VkFence will allow for scheduling multiple transfers simultaneously
	return vkQueueWaitIdle(graphicsQueue);
}