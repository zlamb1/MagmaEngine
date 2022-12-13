#pragma once

#include <memory>

#include "Magma/Render/buffer.h"

namespace Magma {

	class Renderer {

	public:
		void setVertexCount(uint32_t vertexCount);
		void setIndexCount(uint32_t indexCount);
		void setInstanceCount(uint32_t instanceCount);
		void setFirstVertex(uint32_t firstVertex);
		void setFirstInstance(uint32_t firstInstance);
		void setFirstIndex(uint32_t firstIndex);
		void setVertexOffset(int32_t vertexOffset); 
		void setUseIndexing(bool useIndexing); 
		void setIndexBuffer(const std::shared_ptr<Buffer>& indexBuffer);

	protected:
		uint32_t m_VertexCount = 0;
		uint32_t m_IndexCount = 0;
		uint32_t m_InstanceCount = 1;
		uint32_t m_FirstVertex = 0;
		uint32_t m_FirstInstance = 0;
		uint32_t m_FirstIndex = 0;
		int32_t m_VertexOffset = 0;
		bool m_UseIndexing = false;
		std::shared_ptr<Buffer> m_IndexBuffer = nullptr; 

	};

}