#pragma once

#include <GLFW/glfw3.h>

#include "../vulkan/vulkan_api.h"

class Application {

	public:
	
		static Application& instance();

		int run();

	private:

		const uint32_t WIDTH = 800;
		const uint32_t HEIGHT = 600;

		GLFWwindow* window;
		std::unique_ptr<VulkanAPI> vulkanAPI;

		~Application();

		void initWindow();
		void initVulkan();
		void mainLoop();

};