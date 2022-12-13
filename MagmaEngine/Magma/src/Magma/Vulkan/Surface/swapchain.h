#pragma once

#include <algorithm> // std::numeric_limits
#include <vector>

#include <vulkan/vulkan.h>

#include "Magma/Vulkan/Device/vulkan_device.h"
#include "Magma/Vulkan/Setup/vulkan_object.h"

namespace Magma {

	class Swapchain : public VulkanObject {

	public:
		Swapchain(std::shared_ptr<VulkanDevice> device);
		~Swapchain() override;

		VkResult init() override;

		VkSwapchainKHR& getSwapchainKHR();
		VkFormat& getSwapchainImageFormat();
		VkExtent2D& getSwapchainExtent();

		std::vector<VkImage>& getImages();
		std::vector<VkImageView>& getImageViews();

		void deleteImageViews();

	public:
		std::shared_ptr<VulkanDevice> m_Device = nullptr;

	private:
		VkSwapchainKHR m_SwapchainKHR{};
		VkFormat m_SwapchainImageFormat{};
		VkExtent2D m_SwapchainExtent{};

		std::vector<VkImage> m_Images{};
		std::vector<VkImageView> m_ImageViews{};

	};
}