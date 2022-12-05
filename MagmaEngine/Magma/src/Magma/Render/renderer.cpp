#include "renderer.h"

namespace Magma {

	void Renderer::setVertexCount(uint32_t nVertexCount) {
		vertexCount = nVertexCount;
	}

	void Renderer::setIndexCount(uint32_t nIndexCount) {
		indexCount = nIndexCount;
	}

	void Renderer::setInstanceCount(uint32_t nInstanceCount) {
		instanceCount = nInstanceCount;
	}

	void Renderer::setFirstVertex(uint32_t nFirstVertex) {
		firstVertex = nFirstVertex;
	}

	void Renderer::setFirstInstance(uint32_t nFirstInstance) {
		firstInstance = nFirstInstance;
	}

	void Renderer::setFirstIndex(uint32_t nFirstIndex) {
		firstIndex = nFirstIndex;
	}

	void Renderer::setVertexOffset(uint32_t nVertexOffset) {
		vertexOffset = nVertexOffset;
	}

	void Renderer::setUseIndexing(bool nUseIndexing) {
		useIndexing = nUseIndexing;
	}

	void Renderer::setIndexBuffer(std::shared_ptr<Buffer> nIndexBuffer) {
		indexBuffer = nIndexBuffer; 
	}

}