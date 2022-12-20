#pragma once

#include <vector>

#include "glm/glm.hpp"

namespace Magma {

	struct Vertex {
		glm::vec3 m_Position{}, m_Normal{};
		glm::vec2 m_UV{}; 
	};

	class Mesh {

	public:
		std::vector<Vertex>& getVertices() {
			return m_Vertices; 
		}

		std::vector<uint16_t>& getIndices() {
			return m_Indices; 
		}

	protected:
		std::vector<Vertex> m_Vertices{};
		std::vector<uint16_t> m_Indices{}; 

	};

}