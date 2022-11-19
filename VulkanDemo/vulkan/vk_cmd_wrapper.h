#pragma once

#include <vulkan/vulkan.h>

#include "vk_obj.h"
#include "vk_device_wrapper.h"

struct _VkCmdPool : VulkanWrapper {

	QueueFamily* pQueueFamily = nullptr;
	VkDevice* pDevice = nullptr; 

	_VkCmdPool() {};
	~_VkCmdPool();

	VkCommandPool vkCmdPool{};

	VkResult create();

};

struct _VkCmdBuffer {

	VkDevice* pDevice = nullptr;
	_VkCmdPool* pCmdPool = nullptr;

	_VkCmdBuffer() {};

	VkCommandBuffer vkCmdBuffer{};

	VkResult create();

	VkResult recordCmdBuffer();
	void resetCmdBuffer();

};