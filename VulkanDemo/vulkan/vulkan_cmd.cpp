#include "vulkan_cmd.h"

// VulkanCmdPool

VulkanCmdPool::VulkanCmdPool(std::shared_ptr<VulkanDevice> pVulkanDevice) : 
	pVulkanDevice{ pVulkanDevice } {}

VulkanCmdPool::~VulkanCmdPool() {
	vkDestroyCommandPool(pVulkanDevice->getDevice(), vkCmdPool, nullptr);
}

VkResult VulkanCmdPool::init() {
	auto& deviceProfile = VulkanDeviceProfile::instance();

	auto graphicsOptional = deviceProfile.getQueueIndices()[VulkanQueueType::GRAPHICS];
	if (!graphicsOptional.has_value()) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	auto graphicsIndex = graphicsOptional.value();

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = (VkCommandPoolCreateFlags) pFlag;
	poolInfo.queueFamilyIndex = graphicsIndex;
	return vkCreateCommandPool(pVulkanDevice->getDevice(), &poolInfo, pAllocator, &vkCmdPool);
}

VkCommandPool& VulkanCmdPool::getCmdPool() {
	return vkCmdPool;
}

// VulkanCmdBuffer

VulkanCmdBuffer::VulkanCmdBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice,
	std::shared_ptr<VulkanCmdPool> pVulkanCmdPool) : pVulkanDevice{ pVulkanDevice }, 
	pVulkanCmdPool{ pVulkanCmdPool } {};

VulkanCmdBuffer::~VulkanCmdBuffer() {
	vkFreeCommandBuffers(pVulkanDevice->getDevice(), pVulkanCmdPool->getCmdPool(), 1, &vkCmdBuffer);
}

VkResult VulkanCmdBuffer::init() {
	VkCommandBufferAllocateInfo cmdBufferAllocInfo{};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.commandPool = pVulkanCmdPool->getCmdPool();
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferAllocInfo.commandBufferCount = 1;

	return vkAllocateCommandBuffers(pVulkanDevice->getDevice(), &cmdBufferAllocInfo, &vkCmdBuffer);
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