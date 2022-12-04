#pragma once

namespace Magma {

	enum class BufferUsage {
		VERTEX,
		INDEX,
		UNIFORM,
		TRANSFER_SRC,
		TRANSFER_DST
	};

	inline BufferUsage operator|(BufferUsage a, BufferUsage b) {
		return (BufferUsage)((int)a | (int)b);
	}

	class Buffer {

	public:
		Buffer() = default;

		virtual void init() = 0; 

		virtual void* getData() const = 0;
		virtual void setData(void* nData, size_t size) = 0;

		virtual void map() = 0;
		virtual void unmap() = 0;

	protected:
		void* data = nullptr; 

	};

}