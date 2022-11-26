#pragma once

#include <stdexcept>

#include <GLFW/glfw3.h>

#include "../vulkan/vulkan_api.h"
#include "../vulkan/vulkan_buffer_copy.h"

class Application {

public:
	static Application& instance();

	int run();

private:
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	GLFWwindow* glfwWindow = nullptr;
	VulkanAPI vulkanAPI{};

	std::shared_ptr<VulkanBuffer> stagingBuffer, vertexBuffer, indexBuffer;

	float x = 0.0f, y = 1.0f, z = 2.0f;

	~Application();

	void initWindow();
	void initVulkan();
	void mainLoop();

	static void onFramebufferResize(GLFWwindow* window, int width, int height);
};