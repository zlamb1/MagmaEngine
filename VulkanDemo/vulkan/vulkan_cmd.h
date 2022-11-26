#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_object.h"
#include "vulkan_device.h"

enum class VulkanCommandPoolCreateFlag {
	RESET_COMMAND_BUFFER = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	TRANSIENT = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
};

class VulkanCmdPool : public VulkanObject {

public:
	VulkanCmdPool() = default;
	~VulkanCmdPool() override;

	QueueFamily* pQueueFamily = nullptr;
	VkDevice* pDevice = nullptr; 

	VulkanCommandPoolCreateFlag pFlag = VulkanCommandPoolCreateFlag::RESET_COMMAND_BUFFER;

	VkResult init() override;

	VkCommandPool& getCmdPool();

private:
	VkCommandPool vkCmdPool{};

};

class VulkanCmdBuffer : public VulkanObject {

public:
	VulkanCmdBuffer() = default;
	~VulkanCmdBuffer() override;

	VkDevice* pDevice = nullptr;
	VkCommandPool* pCmdPool = nullptr;

	VkResult init() override;

	VkResult record();
	void end();
	void reset();

	VkCommandBuffer& getCmdBuffer();

private:
	VkCommandBuffer vkCmdBuffer; 

};