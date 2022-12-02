#pragma once

#include "Entity/entity.h"

namespace Magma {

	class CameraImpl : public EntityImpl {

	public:
		CameraImpl() = default;
		~CameraImpl() = default;

		const glm::vec3& getUpVector() { return upVector; }
		void setUpVector(glm::vec3 upVector) { this->upVector = upVector; }

		virtual glm::mat4 getViewMatrix() = 0;
		virtual glm::mat4 getPerspectiveMatrix() = 0;

	protected:
		// invert y to match Vulkan's left handed NDC system
		glm::vec3 upVector{ 0.0f, -1.0f, 0.0f };

		float zNear = 0.01f, zFar = 100.0f;

		glm::mat4 viewMatrix{ 1.0f };
		glm::mat4 perspectiveMatrix{ 1.0f };

	};

}