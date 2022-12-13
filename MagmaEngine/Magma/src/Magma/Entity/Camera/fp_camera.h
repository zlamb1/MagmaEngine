#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

namespace Magma {

	class FirstPersonImpl : public CameraImpl, public EventHandler {

	public:
		FirstPersonImpl(WindowInput& input);
		~FirstPersonImpl() override;

		void onUpdate(Timestep step) override;

		void updateViewMatrix() override;

		void onEvent(const WindowResizeEvent& _event) override;
		void onEvent(const WindowFocusEvent& _event) override;
		void onEvent(const MouseButtonEvent& _event) override;
		void onEvent(const MouseMoveEvent& _event) override;

	private:
		[[nodiscard]] glm::vec3 getForwardVec3f(bool includeY) const;
		void setAcceptInput(bool acceptInput);

	private:
		bool m_AcceptInput = false;
		const double m_TargetFps = 60.0;

	};

}