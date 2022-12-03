#include "entity.h"

namespace Magma {

	void EntityImpl::onUpdate(Timestep step) {

	}

	const glm::vec3& EntityImpl::getScale() {
		return scale;
	}

	const glm::vec3& EntityImpl::getPosition() {
		return position;
	}

	const glm::vec3& EntityImpl::getRotation() {
		return rotation;
	}

	void EntityImpl::setScale(glm::vec3 scale) {
		this->scale = scale;
	}

	void EntityImpl::setPosition(glm::vec3 position) {
		this->position = position;
	}

	void EntityImpl::setRotation(glm::vec3 rotation) {
		this->rotation = rotation;
	}

}