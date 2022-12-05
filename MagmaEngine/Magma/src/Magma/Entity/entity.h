#pragma once

#include <glm/glm.hpp>

#include "Magma/Time/timestep.h"

namespace Magma {

	class EntityImpl {

		public:
			virtual void onUpdate(Timestep step);

			const glm::vec3& getScale();
			const glm::vec3& getPosition();
			const glm::vec3& getRotation();

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