#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "Magma/Buffer/buffer.h"

namespace Magma {

	class Renderer {

	public:
		void setVertexCount(uint32_t nVertexCount);
		void setIndexCount(uint32_t nIndexCount);
		void setInstanceCount(uint32_t nInstanceCount);
		void setFirstVertex(uint32_t nFirstVertex);
		void setFirstInstance(uint32_t nFirstInstance);
		void setFirstIndex(uint32_t nFirstIndex);
		void setVertexOffset(uint32_t nVertexOffset); 
		void setUseIndexing(bool nUseIndexing); 
		void setIndexBuffer(std::shared_ptr<Buffer> nIndexBuffer);

	protected:
		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;
		uint32_t instanceCount = 1;
		uint32_t firstVertex = 0;
		uint32_t firstInstance = 0;
		uint32_t firstIndex = 0;
		int32_t vertexOffset = 0;
		bool useIndexing = false;
		std::shared_ptr<Buffer> indexBuffer; 

	};

}