#include "vulkan_cmd.h"

// VulkanCmdPool

VulkanCmdPool::VulkanCmdPool(std::shared_ptr<VulkanDevice> pVulkanDevice) : 
	pVulkanDevice{ pVulkanDevice } {}

VulkanCmdPool::~VulkanCmdPool() {
	vkDestroyCommandPool(pVulkanDevice->getDevice(), vkCmdPool, nullptr);
}

VkResult VulkanCmdPool::init() {
	auto& deviceProfile = DeviceProfile::instance();

	auto graphicsQueueOpt = deviceProfile.getQueueIndices()[VulkanQueueType::GRAPHICS];

	if (!graphicsQueueOpt.has_value()) {
		VulkanLogger::instance().enqueueText("VulkanCmdPool::init", "could not find graphics queue");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = (VkCommandPoolCreateFlags) pFlag;
	poolInfo.queueFamilyIndex = graphicsQueueOpt.value();
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