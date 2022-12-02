#pragma once

#include <algorithm> // std::numeric_limits
#include <limits>    // std::clamp
#include <vector>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "Vulkan/Device/vulkan_device.h"
#include "Vulkan/Setup/vulkan_object.h"

namespace Magma {

	class VulkanSwapchain : public VulkanObject {

	public:
		VulkanSwapchain(std::shared_ptr<VulkanDevice> pVulkanDevice);
		~VulkanSwapchain() override;

		VkResult init() override;

		VkSwapchainKHR& getSwapchainKHR();
		VkFormat& getSwapchainImageFormat();
		VkExtent2D& getSwapchainExtent();

		std::vector<VkImage>& getImages();
		std::vector<VkImageView>& getImageViews();

		void deleteImageViews();

	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;

	private:
		VkSwapchainKHR vkSwapchainKHR{};
		VkFormat vkSwapchainImageFormat{};
		VkExtent2D vkSwapchainExtent{};

		std::vector<VkImage> vkImages{};
		std::vector<VkImageView> vkImageViews{};

	};
}