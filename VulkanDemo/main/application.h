#pragma once

#include <chrono>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window/glfw_window_impl.h"

#include "Vulktrix/vulktrix.h"

class Application {

public:
	static Application& instance();

	int run();

private:
	GLFWWindowImpl windowImpl{};
	VulktrixAPI vulktrixAPI{ windowImpl };

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

};