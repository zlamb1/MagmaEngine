#pragma once

#include <chrono>
#include <stdexcept>

#include "Magma/Application/application.h"

#include "Magma/Entity/Camera/tp_camera.h"
#include "Magma/Entity/Camera/fp_camera.h"
#include "Magma/Event/event.h"
#include "Magma/Geometry/sphere.h"
#include "Magma/Time/timestep.h"
#include "Magma/Vulkan/vulkan_api.h"
#include "Magma/Window/glfw_impl.h"

namespace Magma {

	class SphereApp : public Application {

	public:
		SphereApp() = default;
		~SphereApp();
		
	protected:
		void onNewFrame() override;

		void initWindow() override;
		void initVulkan() override;

	private:
		void updateUniformBuffer();
		glm::vec3 getRandomColor();
		void updateSphereData();
		
	private:
		std::shared_ptr<Buffer> vertexBuffer, indexBuffer, uniformBuffer;
		std::shared_ptr<DescriptorSet> vulkanDescriptorSet;

		float x = 0.0f;
		Timestep frameStep, cameraStep, sphereStep;

		SphereData sphereData; 
		int resolution = 0, sphereMode = 0;
		int MIN_RESOLUTIONS[3] = { 2, 0, 0 };
		int MAX_RESOLUTIONS[3] = { 10, 5, 5 };

		std::shared_ptr<CameraImpl> camera;
		uint32_t cameraIndex = 0;

	};

}