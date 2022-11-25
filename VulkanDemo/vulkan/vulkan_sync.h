#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_object.h"
#include "vulkan_device.h"

class VulkanSemaphore : public VulkanObject {

public:
	VulkanSemaphore() = default;
	~VulkanSemaphore() override;

	VkDevice* pDevice = nullptr;

	VkResult init() override;

	VkSemaphore& getSemaphore();

private:
	VkSemaphore vkSemaphore{};

};

class VulkanFence : public VulkanObject {

public:
	VulkanFence() = default;
	~VulkanFence() override;

	VkDevice* pDevice = nullptr;

	VkResult init() override;

	VkFence& getFence();

private:
	VkFence vkFence;

};

class VulkanRenderSync : public VulkanObject {

public:
	VulkanRenderSync() = default;
	~VulkanRenderSync() override = default;

	VkDevice* pDevice = nullptr;

	VkResult init() override;

	VulkanFence& getFlightFence();
	VulkanSemaphore& getImageSemaphore();
	VulkanSemaphore& getRenderSemaphore();

private:
	VulkanFence flightFence;
	VulkanSemaphore imageSemaphore, renderSemaphore;

};