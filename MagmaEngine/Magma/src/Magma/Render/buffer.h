#pragma once

#include "stdint.h"

namespace Magma {

	enum class BufferUsage {
		VERTEX,
		INDEX,
		UNIFORM,
		TRANSFER_SRC,
		TRANSFER_DST
	};

	inline BufferUsage operator|(BufferUsage a, BufferUsage b) {
		return static_cast<BufferUsage>(static_cast<int>(a) | static_cast<int>(b));
	}

	class Buffer {

	public:
		Buffer() = default;
		virtual ~Buffer() = default; 

		virtual void init() = 0; 

		[[nodiscard]] virtual uint64_t getSize() const = 0; 

		[[nodiscard]] virtual void* get() const = 0;
		virtual void set(void* nData, size_t size) = 0;

		virtual void map() = 0;
		virtual void unmap() = 0;

	protected:
		void* m_Data = nullptr; 

	};

}