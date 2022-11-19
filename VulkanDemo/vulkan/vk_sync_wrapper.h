#pragma once

#include <vulkan/vulkan.h>

#include "vk_obj.h"
#include "vk_device_wrapper.h"

struct _VkSemaphore : VulkanWrapper {

	_VkSemaphore() {}
	~_VkSemaphore();

	VkDevice* pDevice = nullptr;

	VkSemaphore vkSemaphore{};

	VkResult create();

};

struct _VkFence : VulkanWrapper {

	_VkFence() {}
	~_VkFence();

	VkDevice* pDevice = nullptr;

	VkFence vkFence{};

	VkResult create();

};

struct _VkRenderSync : VulkanWrapper {

	_VkRenderSync() {}

	VkDevice* pDevice = nullptr;

	_VkFence _vkFlightFence{};
	_VkSemaphore _vkImageSemaphore{};
	_VkSemaphore _vkRenderSemaphore{};

	VkResult create();

};