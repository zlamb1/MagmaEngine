#include "renderer.h"

namespace Magma {

	void Renderer::setVertexCount(const uint32_t vertexCount) {
		m_VertexCount = vertexCount;
	}

	void Renderer::setIndexCount(const uint32_t indexCount) {
		m_IndexCount = indexCount;
	}

	void Renderer::setInstanceCount(const uint32_t instanceCount) {
		m_InstanceCount = instanceCount;
	}

	void Renderer::setFirstVertex(const uint32_t firstVertex) {
		m_FirstVertex = firstVertex;
	}

	void Renderer::setFirstInstance(const uint32_t firstInstance) {
		m_FirstInstance = firstInstance;
	}

	void Renderer::setFirstIndex(const uint32_t firstIndex) {
		m_FirstIndex = firstIndex;
	}

	void Renderer::setVertexOffset(const int32_t vertexOffset) {
		m_VertexOffset = vertexOffset;
	}

	void Renderer::setUseIndexing(const bool useIndexing) {
		m_UseIndexing = useIndexing;
	}

	void Renderer::setIndexBuffer(const std::shared_ptr<Buffer>& indexBuffer) {
		m_IndexBuffer = indexBuffer;
	}

}