#pragma once

#include <vulkan/vulkan.h>

#include "Magma/Vulkan/Device/vulkan_device.h"

namespace Magma {

	class VulkanSemaphore : public VulkanObject {

	public:
		VulkanSemaphore() = default;
		~VulkanSemaphore() override;


		VkResult init() override;

		VkSemaphore& getSemaphore();

	public:
		VkDevice* pDevice = nullptr;

	private:
		VkSemaphore vkSemaphore{};

	};

	class VulkanFence : public VulkanObject {

	public:
		VulkanFence() = default;
		~VulkanFence() override;

		VkResult init() override;

		VkFence& getFence();
	
	public:
		VkDevice* pDevice = nullptr;

	private:
		VkFence vkFence;

	};

	class VulkanRenderSync : public VulkanObject {

	public:
		VulkanRenderSync() = default;
		~VulkanRenderSync() override = default;

		VkResult init() override;

		VulkanFence& getFlightFence();
		VulkanSemaphore& getImageSemaphore();
		VulkanSemaphore& getRenderSemaphore();

	public:
		VkDevice* pDevice = nullptr;

	private:
		VulkanFence flightFence;
		VulkanSemaphore imageSemaphore, renderSemaphore;

	};

}