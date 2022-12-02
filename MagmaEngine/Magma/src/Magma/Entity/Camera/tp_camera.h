#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

#include "Window/window.h"

namespace Magma {

	typedef glm::dvec2 MousePosition;

	class ThirdPersonImpl : public CameraImpl {
	public:
		ThirdPersonImpl(WindowImpl& window);

		glm::mat4 getViewMatrix() override;
		glm::mat4 getPerspectiveMatrix() override; 

	private:
		float getZoomSpeed();

	private:
		bool rightMouseBtnPressed = false;
		MousePosition lastMousePos{ -1.0f, -1.0f };

		float radius = 5.0f;

	};

}