#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace Magma {

	struct ImageData {
		unsigned char* m_Pixels = nullptr; 
		int32_t m_Width = 0, m_Height = 0, m_Channels = 0;
		virtual ~ImageData() = default; 
	};

	class ImageLoader {

	public:
		virtual ~ImageLoader() = default; 
		virtual std::shared_ptr<ImageData> loadImage(const std::string& path) = 0; 

	};

}
