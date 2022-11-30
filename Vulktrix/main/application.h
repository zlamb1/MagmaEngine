#pragma once

#include <chrono>
#include <stdexcept>

#include "Window/glfw_impl.h"

#include "Vulktrix/vulktrix.h"

#include "Entity/Camera/tp_camera.h"

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

	Camera::ThirdPersonImpl thirdPersonImpl{ windowImpl };

	void updateUniformBuffer();

	void initWindow();
	void initVulkan();
	void mainLoop();

};