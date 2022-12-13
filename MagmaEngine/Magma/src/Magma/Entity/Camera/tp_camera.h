#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

namespace Magma {

	typedef glm::dvec2 MousePosition;

	class ThirdPersonImpl : public CameraImpl, public EventHandler {

	public:
		ThirdPersonImpl(WindowInput& input);
		~ThirdPersonImpl() override;

		void updateViewMatrix() override;

		void onEvent(const WindowResizeEvent& _event) override;
		void onEvent(const MouseMoveEvent& _event) override;
		void onEvent(const MouseScrollEvent& _event) override;
			
	private:
		[[nodiscard]] float getZoomSpeed() const;

	private:
		MousePosition m_LastMousePosition{ -1.0f, -1.0f };
		float m_ViewRadius = 5.0f;

	};

}