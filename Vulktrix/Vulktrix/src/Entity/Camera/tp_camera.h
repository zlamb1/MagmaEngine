#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

#include "Window/window.h"

namespace Camera {
	class ThirdPersonImpl : public Camera::CameraImpl {
		public:
			ThirdPersonImpl(Window::WindowImpl& window);

			glm::mat4 getViewMatrix() override;

		private:
			bool rightMouseBtnPressed = false;
			double lastX = -1.0, lastY = -1.0;

			float radius = 5.0f;
	};
}