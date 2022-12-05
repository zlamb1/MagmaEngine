#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

#include "Magma/Window/window_input.h"

namespace Magma {

	typedef glm::dvec2 MousePosition;

	class ThirdPersonImpl : public CameraImpl, public EventHandler {

	public:
		ThirdPersonImpl(WindowInput& input);
		~ThirdPersonImpl();

		void updateViewMat4f() override;
		void updatePerspectiveMat4f() override;

		const glm::mat4& getViewMat4f() const override;
		const glm::mat4& getPerspectiveMat4f() const override; 

		void onEvent(const WindowResizeEvent& _event) override;
		void onEvent(const MouseMoveEvent& _event) override;
		void onEvent(const MouseScrollEvent& _event) override;
			
	private:
		float getZoomSpeed();

	private:
		WindowInput& input;
		MousePosition lastMousePos{ -1.0f, -1.0f };
		float radius = 5.0f;

	};

}