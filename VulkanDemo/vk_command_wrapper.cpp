#include "vk_command_wrapper.h"

// Public

VkCommandWrapper::VkCommandWrapper(VkDeviceWrapper& _vkDeviceWrapper) :
	vkDeviceWrapper{ _vkDeviceWrapper }
{

}

VkCommandWrapper::~VkCommandWrapper() {
	vkDestroyCommandPool(vkDeviceWrapper.getLogicalDevice(), 
		vkCommandPool, nullptr);
}

VkCommandBuffer& VkCommandWrapper::getCommandBuffer() {
	return vkCommandBuffer;
}

void VkCommandWrapper::recordCommandBuffer(
	uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // optional
	beginInfo.pInheritanceInfo = nullptr; // optional

	if (vkBeginCommandBuffer(vkCommandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}
}

void VkCommandWrapper::resetCommandBuffer() {
	vkResetCommandBuffer(vkCommandBuffer, 0);
}

void VkCommandWrapper::init() {
	initCommandPool();
	initCommandBuffer();
}

// Private

void VkCommandWrapper::initCommandPool() {
	QueueFamily family = vkDeviceWrapper.findQueueFamily(VK_QUEUE_GRAPHICS_BIT);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = family.getGraphics();

	if (vkCreateCommandPool(vkDeviceWrapper.getLogicalDevice(),
		&poolInfo, nullptr, &vkCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void VkCommandWrapper::initCommandBuffer() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vkCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(vkDeviceWrapper.getLogicalDevice(), 
		&allocInfo, &vkCommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}