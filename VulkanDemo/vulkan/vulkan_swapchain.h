#pragma once

#include <algorithm> // std::numeric_limits
#include <limits>    // std::clamp
#include <vector>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "vulkan_device.h"
#include "vulkan_object.h"

class VulkanSwapchain : public VulkanObject {

public:
	VulkanSwapchain(std::shared_ptr<VulkanDevice> pVulkanDevice);
	~VulkanSwapchain() override;

	std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;

	VkResult init() override;

	VkSwapchainKHR& getSwapchainKHR();
	VkFormat& getSwapchainImageFormat();
	VkExtent2D& getSwapchainExtent();

	std::vector<VkImage>& getImages();
	std::vector<VkImageView>& getImageViews();

	void deleteImageViews();

private:
	VkSwapchainKHR vkSwapchainKHR{};
	VkFormat vkSwapchainImageFormat{};
	VkExtent2D vkSwapchainExtent{};

	std::vector<VkImage> vkImages{};
	std::vector<VkImageView> vkImageViews{};

};