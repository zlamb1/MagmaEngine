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

		[[nodiscard]] uint32_t getVertexCount() const {
			return m_VertexCount; 
		}

		[[nodiscard]] uint32_t getIndexCount() const {
			return m_IndexCount; 
		}

		void setVertexCount(uint32_t vertexCount) {
			m_VertexCount = vertexCount; 
		}

		void setIndexCount(uint32_t indexCount) {
			m_IndexCount = indexCount; 
		}

	protected:
		uint32_t m_VertexCount = 0, m_IndexCount = 0;

		std::vector<Vertex> m_Vertices{};
		std::vector<uint16_t> m_Indices{}; 

	};

}