#pragma once

#include <chrono>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

	std::shared_ptr<VulkanBuffer> stagingBuffer, vertexBuffer, indexBuffer, uboBuffer;
	std::shared_ptr<VulkanDeviceMemory> stagingMemory, vertexMemory, indexMemory, uboMemory;
	
	std::shared_ptr<VulkanDescriptorSet> vulkanDescriptorSet;

	float x = 0.0f;

	~Application();

	void updateUniformBuffer();

	void initWindow();
	void initVulkan();
	void mainLoop();

	static void onFramebufferResize(GLFWwindow* window, int width, int height);
};