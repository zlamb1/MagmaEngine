#pragma once

#include <chrono>
#include <stdexcept>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window/glfw_impl.h"

#include "Vulktrix/vulktrix.h"

class Application {

public:
	static Application& instance();

	int run();

private:
	Application() = default;
	~Application();

	Application& operator=(const Application& o) = default;

	Window::GLFWImpl windowImpl = Window::GLFWImpl();
	VulktrixAPI vulktrixAPI{ windowImpl };

	std::shared_ptr<VulkanBuffer> stagingBuffer, vertexBuffer, indexBuffer, uboBuffer;
	std::shared_ptr<VulkanDeviceMemory> stagingMemory, vertexMemory, indexMemory, uboMemory;
	
	std::shared_ptr<DescriptorSet> vulkanDescriptorSet;

	float x = 0.0f;

	int frames = 0;
	double lastTime = 0;

	float yaw = glm::radians(30.0f), pitch = glm::radians(30.0f), radius = 5.0f;
	float lastX = -1.0f, lastY = -1.0f;

	bool lMousePressed = false, rMousePressed = false;

	void updateUniformBuffer();

	void initWindow();
	void initVulkan();
	void mainLoop();

};