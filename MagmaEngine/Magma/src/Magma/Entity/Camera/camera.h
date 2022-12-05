#pragma once

#include "Magma/Entity/entity.h"

namespace Magma {

	class CameraImpl : public EntityImpl {

	public:
		CameraImpl() = default;
		~CameraImpl() = default;

		const glm::vec3& getUpVector() { return upVec3f; }
		void setUpVector(glm::vec3 upVec3f) { this->upVec3f = upVec3f; }

		virtual void updateViewMat4f() = 0;
		virtual void updatePerspectiveMat4f() = 0;

		virtual const glm::mat4& getViewMat4f() const = 0;
		virtual const glm::mat4& getPerspectiveMat4f() const = 0;

	protected:
		// invert y to match Vulkan's left handed NDC system
		glm::vec3 upVec3f{ 0.0f, -1.0f, 0.0f };

		float fov = 45.0f, zNear = 0.01f, zFar = 100.0f;

		glm::mat4 viewMat4f{ 1.0f };
		glm::mat4 perspectiveMat4f{ 1.0f };

	};

}