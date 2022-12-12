#pragma once

#include <chrono>

#include "Magma/Application/application.h"

#include "Magma/Entity/Camera/fp_camera.h"
#include "Magma/Geometry/sphere.h"
#include "Magma/Image/image.h"
#include "Magma/Time/timestep.h"
#include "Magma/Vulkan/vulkan_api.h"

namespace Magma {

	class SphereApp : public Application {

	public:
		SphereApp() = default;
		virtual ~SphereApp();

	protected:
		void onNewFrame() override;

		void initWindow() override;
		void initVulkan() override;

	private:
		void updateUniformBuffer() const;
		void updateSphereData();
		void createTexture();

	private:
		std::shared_ptr<Buffer> m_VertexBuffer, m_UboBuffer, m_TimeBuffer, m_TextureBuffer;
		std::shared_ptr<DescriptorSet> m_DescriptorSet;

		std::shared_ptr<Image> m_TextureImage;
		std::shared_ptr<VulkanImageView> m_TextureImageView;
		std::shared_ptr<Sampler> m_TextureSampler;

		Timestep m_TimeStep, m_FrameStep, m_CameraStep, m_SphereStep;

		SphereData m_SphereData;
		int m_Resolution = 0, m_SphereMode = 0;

		const int MIN_RESOLUTIONS[3] = {2, 0, 0};
		const int MAX_RESOLUTIONS[3] = {10, 5, 5};

		std::shared_ptr<CameraImpl> m_Camera;
		uint32_t m_CameraIndex = 0;

	};

}
