#pragma once

#include <chrono>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include "../vulkan/vulkan_api.h"
#include "../vulkan/vulkan_buffer_copy.h"

#include "../vulkan/glfw_window_impl.h"

class Application {

public:
	static Application& instance();

	int run();

private:
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	GLFWWindowImpl windowImpl{};
	VulkanAPI vulkanAPI{ windowImpl };

	std::shared_ptr<VulkanBuffer> stagingBuffer, vertexBuffer, indexBuffer, uboBuffer;
	std::shared_ptr<VulkanDeviceMemory> stagingMemory, vertexMemory, indexMemory, uboMemory;
	
	std::shared_ptr<VulkanDescriptorSet> vulkanDescriptorSet;

	float x = 0.0f;

	int frames = 0;
	double lastTime = 0;

	~Application();

	void updateUniformBuffer();

	void initWindow();
	void initVulkan();
	void mainLoop();

	static void onFramebufferResize(GLFWwindow* window, int width, int height);
};