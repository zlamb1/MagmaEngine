#include "entity.h"

namespace Magma {

	const glm::vec3& EntityImpl::getScale() const {
		return m_Scale;
	}

	const glm::vec3& EntityImpl::getPosition() const {
		return m_Position;
	}

	const glm::vec3& EntityImpl::getRotation() const {
		return m_Rotation;
	}

	void EntityImpl::setScale(glm::vec3 scale) {
		m_Scale = scale;
	}

	void EntityImpl::setPosition(glm::vec3 position) {
		m_Position = position;
	}

	void EntityImpl::setRotation(glm::vec3 rotation) {
		m_Rotation = rotation;
	}

}