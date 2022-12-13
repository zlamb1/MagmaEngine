#pragma once

#include "Magma/Entity/entity.h"
#include "Magma/Window/window_input.h"

namespace Magma {

	class CameraImpl : public EntityImpl {

	public:
		CameraImpl(WindowInput& input) : m_Input{input} {};
		virtual ~CameraImpl() = default;

		[[nodiscard]] const glm::vec3& getUpVector() const { return m_UpVector; }
		void setUpVector(const glm::vec3 upVector) { m_UpVector = upVector; }

		[[nodiscard]] virtual const glm::mat4& getViewMatrix() const { return m_ViewMatrix; };
		[[nodiscard]] virtual const glm::mat4& getPerspectiveMatrix() const { return m_PerspectiveMatrix; };

		virtual void updateViewMatrix() = 0;
		virtual void updatePerspectiveMatrix() {
			const auto [width, height] = m_Input.getWindowImpl().getSize();
			m_PerspectiveMatrix = glm::perspective(glm::radians(m_FOV),
				static_cast<float>(width) / static_cast<float>(height), m_NearPlane, m_FarPlane);
		}

	protected:
		WindowInput& m_Input;

		// invert y to match Vulkan's left handed NDC system
		glm::vec3 m_UpVector{ 0.0f, -1.0f, 0.0f };

		float m_FOV = 45.0f, m_NearPlane = 0.01f, m_FarPlane = 100.0f;

		glm::mat4 m_ViewMatrix{ 1.0f };
		glm::mat4 m_PerspectiveMatrix{ 1.0f };

	};

}