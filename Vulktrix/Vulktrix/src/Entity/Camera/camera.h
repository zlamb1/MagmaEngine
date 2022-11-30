#pragma once

#include "Entity/entity.h"

namespace Camera {
	class CameraImpl : public Entity::EntityImpl {
		public:
			CameraImpl() = default;
			~CameraImpl() = default;

			const glm::vec3& getUpVector() { return upVector; }
			void setUpVector(glm::vec3 upVector) { this->upVector = upVector; }

			virtual glm::mat4 getViewMatrix() { return glm::mat4{ 1.0f }; };

		protected:
			// invert up to match Vulkan's left handed NDC system
			glm::vec3 upVector{ 0.0f, -1.0f, 0.0f };
	};
}