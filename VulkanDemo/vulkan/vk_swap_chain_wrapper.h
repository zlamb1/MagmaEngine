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
			VkDeviceWrapper& _vkDeviceWrapper);
		~VkSwapChainWrapper();

		VkFormat& getSwapChainImageFormat();
		VkExtent2D& getSwapChainExtent();
		std::vector<VkImageView>& getImageViews();

		VkSwapchainKHR& getSwapchain();

	private:

		GLFWwindow& window;
		VkDeviceWrapper& vkDeviceWrapper;

		VkSwapchainKHR vkSwapChain; 

		VkFormat vkSwapChainImageFormat;
		VkExtent2D vkSwapChainExtent;

		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		void initSwapChain(VkSurfaceWrapper& _vkSurfaceWrapper);
		void initImageViews();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(
			const VkSurfaceCapabilitiesKHR& capabilities);
};