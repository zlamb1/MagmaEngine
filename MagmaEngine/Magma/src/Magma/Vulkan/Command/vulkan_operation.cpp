#include "vulkan_operation.h"

namespace Magma {

	VulkanOperation::VulkanOperation(std::shared_ptr<VulkanDevice> pVulkanDevice) :
		pVulkanDevice{ pVulkanDevice }, pVulkanCmdPool{ std::make_shared<VulkanCmdPool>(pVulkanDevice) } {
		pVulkanCmdPool->pFlag = VulkanCommandPoolCreateFlag::RESET_COMMAND_BUFFER;
		pVulkanCmdPool->init();

		vulkanCmdBuffer = std::make_shared<VulkanCmdBuffer>(pVulkanDevice, pVulkanCmdPool);
		vulkanCmdBuffer->init();
	}

	void VulkanOperation::record() {
		vulkanCmdBuffer->record();
	}

	VkResult VulkanOperation::submit() {
		vkEndCommandBuffer(vulkanCmdBuffer->getCmdBuffer());

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vulkanCmdBuffer->getCmdBuffer();

		const auto graphicsQueueOpt = DeviceProfile::getQueue(VulkanQueueType::GRAPHICS);
		if (!graphicsQueueOpt.has_value()) {
			Z_LOG_TXT("VulkanBufferCopy::init", "could not find graphics queue");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		auto queueSubmit = vkQueueSubmit(graphicsQueueOpt.value(), 1, &submitInfo, 
			VK_NULL_HANDLE);
		Z_LOG_OBJ("VulkanOperation::record", queueSubmit);
		if (queueSubmit != VK_SUCCESS)
			return queueSubmit;

		auto queueWaitIdle = vkQueueWaitIdle(graphicsQueueOpt.value());
		Z_LOG_OBJ("VulkanOperation::record", queueWaitIdle);

		return queueWaitIdle;
	}

	TransitionImage::TransitionImage(std::shared_ptr<VulkanDevice> pVulkanDevice)
		: VulkanOperation{ pVulkanDevice } {}

	VkResult TransitionImage::init() {
		record();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = pOldLayout;
		barrier.newLayout = pNewLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		const auto image = std::dynamic_pointer_cast<VulkanImage>(pImage);
		barrier.image = image->getImage();
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags srcStage, dstStage;

		if (pOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && 
			pNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (pOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && 
			pNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			Z_LOG_TXT("TransitionImage::init", "unsupported image layout transition");
			return VK_ERROR_INITIALIZATION_FAILED; 
		}

		vkCmdPipelineBarrier(vulkanCmdBuffer->getCmdBuffer(),
			srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		return submit();
	}

	CopyBufferToImage::CopyBufferToImage(std::shared_ptr<VulkanDevice> pVulkanDevice) :
		VulkanOperation(pVulkanDevice) {}

	VkResult CopyBufferToImage::init() {
		record();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { (uint32_t)pImage->pExtentWidth, (uint32_t)pImage->pExtentHeight, 1 };

		auto vImage = dynamic_pointer_cast<VulkanImage>(pImage);
		auto vBuffer = dynamic_pointer_cast<VmaBuffer>(pBuffer);
		vkCmdCopyBufferToImage(vulkanCmdBuffer->getCmdBuffer(),
			vBuffer->getBuffer(), vImage->getImage(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		return submit();
	}

}