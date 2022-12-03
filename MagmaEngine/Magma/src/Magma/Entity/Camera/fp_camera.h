#pragma once

#include <iostream>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

#include "Window/window_input.h"

namespace Magma {

	class FirstPersonImpl : public CameraImpl, public EventHandler {

	public:
		FirstPersonImpl(WindowInput& input);
		~FirstPersonImpl();

		void onUpdate(Timestep step) override;

		void updateViewMat4f() override;
		void updatePerspectiveMat4f() override;

		const glm::mat4& getViewMat4f() const override;
		const glm::mat4& getPerspectiveMat4f() const override;

		void onEvent(const WindowResizeEvent& _event) override;
		void onEvent(const WindowFocusEvent& _event) override;
		void onEvent(const MouseButtonEvent& _event) override;
		void onEvent(const MouseMoveEvent& _event) override;

	private:
		glm::vec3 getForwardVec3f();
		void setAcceptInput(bool acceptInput);

	private:
		WindowInput& input;
		bool acceptInput = false;
		const double targetFps = 60.0;

	};

}