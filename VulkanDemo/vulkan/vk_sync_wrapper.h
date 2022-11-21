#pragma once

#include <vulkan/vulkan.h>

#include "vk_obj.h"
#include "vk_device_wrapper.h"

struct _VkSemaphore : VulkanWrapper {

	_VkSemaphore();
	~_VkSemaphore() override;

	VkDevice* pDevice = nullptr;

	VkSemaphore vkSemaphore{};

	VkResult create() override;

};

struct _VkFence : VulkanWrapper {

	_VkFence();
	~_VkFence() override;

	VkDevice* pDevice = nullptr;

	VkFence vkFence{};

	VkResult create() override;

};

struct _VkRenderSync : VulkanWrapper {

	_VkRenderSync();
	~_VkRenderSync() override;

	VkDevice* pDevice = nullptr;

	_VkFence _vkFlightFence{};
	_VkSemaphore _vkImageSemaphore{};
	_VkSemaphore _vkRenderSemaphore{};

	VkResult create() override;

};