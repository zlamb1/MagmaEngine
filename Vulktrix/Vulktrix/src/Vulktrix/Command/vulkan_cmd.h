#pragma once

#include "Vulktrix/Device/vulkan_device.h"

enum class VulkanCommandPoolCreateFlag {
	RESET_COMMAND_BUFFER = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	TRANSIENT = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
};

class VulkanCmdPool : public VulkanObject {

public:
	VulkanCmdPool(std::shared_ptr<VulkanDevice> pVulkanDevice);
	~VulkanCmdPool() override;
	
	std::shared_ptr<VulkanDevice> pVulkanDevice;
	VulkanCommandPoolCreateFlag pFlag = VulkanCommandPoolCreateFlag::RESET_COMMAND_BUFFER;

	VkResult init() override;

	VkCommandPool& getCmdPool();

private:
	VkCommandPool vkCmdPool{};

};

class VulkanCmdBuffer : public VulkanObject {

public:
	VulkanCmdBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice, 
		std::shared_ptr<VulkanCmdPool> pVulkanCmdPool);
	~VulkanCmdBuffer() override;

	std::shared_ptr<VulkanDevice> pVulkanDevice;
	std::shared_ptr<VulkanCmdPool> pVulkanCmdPool;

	VkResult init() override;

	VkResult record();
	void end();
	void reset();

	VkCommandBuffer& getCmdBuffer();

private:
	VkCommandBuffer vkCmdBuffer{};

};