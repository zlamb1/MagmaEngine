#pragma once

#include <vulkan/vulkan.h>

#include "vk_obj.h"
#include "vk_device_wrapper.h"

struct VkCmdPoolWrapper : VulkanWrapper {

	QueueFamily* pQueueFamily = nullptr;
	VkDevice* pDevice = nullptr; 

	VkCmdPoolWrapper() {};
	~VkCmdPoolWrapper();

	VkResult create();

	VkCommandPool vkCmdPool{};

};

struct VkCmdBufferWrapper {

	VkDevice* pDevice = nullptr;
	VkCmdPoolWrapper* pCmdPool = nullptr;

	VkCmdBufferWrapper() {};
	~VkCmdBufferWrapper();

	VkResult create();

	VkResult recordCmdBuffer();
	void resetCmdBuffer();

	VkCommandBuffer vkCmdBuffer{};

};