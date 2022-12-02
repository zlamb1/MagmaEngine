#pragma once

#include <chrono>
#include <stdexcept>

#include "Window/glfw_impl.h"

#include "Vulkan/vulkan_api.h"

#include "Entity/Camera/tp_camera.h"

#include "Event/event.h"

#include "Time/timestep.h"

namespace Magma {

	class Application {

	public:
		static Application& instance();

		int run();

	private:
		Application() = default;
		~Application();

		Application& operator=(const Application& o) = default;

		void updateUniformBuffer();
		void initWindow();
		void initVulkan();
		void mainLoop();

	private:
		GLFWImpl windowImpl = GLFWImpl();
		VulkanAPI vulkanAPI{ windowImpl };

		std::shared_ptr<VulkanBuffer> stagingBuffer, vertexBuffer, indexBuffer, uboBuffer;
		std::shared_ptr<VulkanDeviceMemory> stagingMemory, vertexMemory, indexMemory, uboMemory;

		std::shared_ptr<DescriptorSet> vulkanDescriptorSet;

		float x = 0.0f;

		Timestep step{}, measure{};

		ThirdPersonImpl thirdPersonImpl{ windowImpl };

		Magma::EventDispatcher dispatcher{};

	};

}