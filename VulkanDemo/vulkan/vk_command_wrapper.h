#pragma once

#include <vulkan/vulkan.h>

#include "vk_device_wrapper.h"

class VkCommandWrapper {

	public:

		VkCommandWrapper(VkDeviceWrapper& _vkDeviceWrapper);
		~VkCommandWrapper();

		VkCommandBuffer& getCommandBuffer();

		void recordCommandBuffer(uint32_t imageIndex);
		void resetCommandBuffer();

		void init();

	private:

		VkDeviceWrapper& vkDeviceWrapper;

		VkCommandPool vkCommandPool;
		VkCommandBuffer vkCommandBuffer;

		void initCommandPool();
		void initCommandBuffer();

};