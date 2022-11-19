#pragma once

#include <algorithm> // std::numeric_limits
#include <limits>    // std::clamp
#include <vector>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "vk_device_wrapper.h"

class VkSwapChainWrapper {

	public:

		VkSwapChainWrapper(GLFWwindow& _window,
			VkSurfaceWrapper& _vkSurfaceWrapper,
			_VkDevice& _vkDevice);
		~VkSwapChainWrapper();

		VkFormat& getSwapChainImageFormat();
		VkExtent2D& getSwapChainExtent();
		std::vector<VkImageView>& getImageViews();

		VkSwapchainKHR& getSwapchain();

	private:

		GLFWwindow& glfwWindow;
		_VkDevice& _vkDevice;

		VkSwapchainKHR vkSwapchainKHR; 

		VkFormat vkSwapchainImageFormat;
		VkExtent2D vkSwapchainExtent;

		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> swapchainImageViews;

		void initSwapChain(VkSurfaceWrapper& _vkSurfaceWrapper);
		void initImageViews();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(
			const VkSurfaceCapabilitiesKHR& capabilities);
};