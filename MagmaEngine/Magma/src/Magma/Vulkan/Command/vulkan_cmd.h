#pragma once

#include "Magma/Vulkan/Device/vulkan_device.h"

namespace Magma {

	enum class VulkanCommandPoolCreateFlag {
		RESET_COMMAND_BUFFER = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		TRANSIENT = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
	};

	class VulkanCmdPool : public VulkanObject {

	public:
		VulkanCmdPool(std::shared_ptr<VulkanDevice> pVulkanDevice);
		~VulkanCmdPool() override;

		VkResult init() override;

		VkCommandPool& getCmdPool();

	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice;
		VulkanCommandPoolCreateFlag pFlag = VulkanCommandPoolCreateFlag::RESET_COMMAND_BUFFER;

	private:
		VkCommandPool vkCmdPool{};

	};

	class VulkanCmdBuffer : public VulkanObject {

	public:
		VulkanCmdBuffer(std::shared_ptr<VulkanDevice> pVulkanDevice,
			std::shared_ptr<VulkanCmdPool> pVulkanCmdPool);
		~VulkanCmdBuffer() override;

		VkResult init() override;

		VkResult record(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		void end();
		void reset();

		VkCommandBuffer& getCmdBuffer();

	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice;
		std::shared_ptr<VulkanCmdPool> pVulkanCmdPool;

	private:
		VkCommandBuffer vkCmdBuffer{};

	};

}