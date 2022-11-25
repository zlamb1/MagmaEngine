#pragma once

#include <stdexcept>

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
	VulkanAPI vulkanAPI{};

	VulkanBuffer* vulkanBufferHandle;

	float x = 0.0f;
	float y = 1.0f;
	float z = 2.0f;

	~Application();

	void initWindow();
	void initVulkan();
	void mainLoop();

	static void onFramebufferResize(GLFWwindow* window, int width, int height);
};