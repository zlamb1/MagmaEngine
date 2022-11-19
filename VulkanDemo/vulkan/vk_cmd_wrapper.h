#pragma once

#include <vulkan/vulkan.h>

#include "vk_obj.h"
#include "vk_device_wrapper.h"

struct VkCmdPoolWrapper : VulkanWrapper {

	QueueFamily* pQueueFamily = nullptr;
	VkDevice* pDevice = nullptr; 

	VkCmdPoolWrapper() {};
	~VkCmdPoolWrapper();

	VkCommandPool vkCmdPool{};

	VkResult create();

};

struct VkCmdBufferWrapper {

	VkDevice* pDevice = nullptr;
	VkCmdPoolWrapper* pCmdPool = nullptr;

	VkCmdBufferWrapper() {};
	~VkCmdBufferWrapper();

	VkCommandBuffer vkCmdBuffer{};

	VkResult create();

	VkResult recordCmdBuffer();
	void resetCmdBuffer();

};