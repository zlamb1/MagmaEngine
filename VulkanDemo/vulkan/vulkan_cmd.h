#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_object.h"
#include "vulkan_device.h"

class VulkanCmdPool : public VulkanObject {

public:
	VulkanCmdPool() = default;
	~VulkanCmdPool() override;

	QueueFamily* pQueueFamily = nullptr;
	VkDevice* pDevice = nullptr; 

	VkResult init() override;

	VkCommandPool& getCmdPool();

private:
	VkCommandPool vkCmdPool{};

};

class VulkanCmdBuffer : public VulkanObject {

public:
	VulkanCmdBuffer() = default;
	~VulkanCmdBuffer() override = default;

	VkDevice* pDevice = nullptr;
	VkCommandPool* pCmdPool = nullptr;

	VkResult init() override;

	VkResult record();
	void reset();

	VkCommandBuffer& getCmdBuffer();

private:
	VkCommandBuffer vkCmdBuffer; 

};