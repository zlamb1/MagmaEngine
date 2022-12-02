#pragma once

#include <glm/glm.hpp>

namespace Magma {

	class EntityImpl {

		public:
			glm::vec3 getScale();
			glm::vec3 getPosition();
			glm::vec3 getRotation();

			void setScale(glm::vec3 scale);
			void setPosition(glm::vec3 position);
			void setRotation(glm::vec3 rotation);

		protected:
			glm::vec3 scale{ 1.0f };
			glm::vec3 position{ 0.0f };
			glm::vec3 rotation{ 0.0f };

			EntityImpl* parent = nullptr;

	};

}