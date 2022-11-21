#pragma once

#include <algorithm> // std::numeric_limits
#include <limits>    // std::clamp
#include <vector>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "vk_device_wrapper.h"
#include "vk_obj.h"

struct _VkSwapchain : VulkanWrapper {

	_VkSwapchain();
	~_VkSwapchain();

	GLFWwindow* pWindow = nullptr;
	_VkDevice* _pDevice = nullptr;
	VkSurfaceKHR* pSurfaceKHR = nullptr;

	VkSwapchainKHR vkSwapchainKHR{};

	VkFormat vkSwapchainImageFormat{};
	VkExtent2D vkSwapchainExtent{};
	std::vector<VkImage> vkImages{};
	std::vector<VkImageView> vkImageViews{};

	VkResult create();

	void deleteImageViews();

};