#pragma once

#include <iostream>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

class VkSurfaceWrapper {

	public:

		VkSurfaceWrapper(VkInstance& vkInstance, GLFWwindow& window);
		~VkSurfaceWrapper();

		VkSurfaceKHR& getSurface();

	private:

		VkInstance& vkInstance;
		GLFWwindow& window;

		VkSurfaceKHR vkSurface;

		void initSurface();

};