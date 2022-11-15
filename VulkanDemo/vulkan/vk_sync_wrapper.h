#pragma once

#include <vulkan/vulkan.h>

#include "vk_device_wrapper.h"

class VkSyncWrapper {

	public:

		VkSyncWrapper(VkDeviceWrapper& _vkDeviceWrapper);
		~VkSyncWrapper();

		VkFence& getFlightFence();
		VkSemaphore& getImageSemaphore();
		VkSemaphore& getRenderSemaphore();

		void init();

	private:

		VkDeviceWrapper& vkDeviceWrapper;

		VkSemaphore vkImageSemaphore;
		VkSemaphore vkRenderSemaphore;
		VkFence vkFlightFence;

};