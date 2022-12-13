#pragma once

#include <glm/glm.hpp>

#include "Magma/Time/timestep.h"

namespace Magma {

	class EntityImpl {

		public:
			virtual void onUpdate(Timestep step) {};

			[[nodiscard]] const glm::vec3& getScale() const;
			[[nodiscard]] const glm::vec3& getPosition() const;
			[[nodiscard]] const glm::vec3& getRotation() const;

			void setScale(glm::vec3 scale);
			void setPosition(glm::vec3 position);
			void setRotation(glm::vec3 rotation);

		protected:
			glm::vec3 m_Scale{ 1.0f };
			glm::vec3 m_Position{ 0.0f };
			glm::vec3 m_Rotation{ 0.0f };

			EntityImpl* m_Parent = nullptr;

	};

}