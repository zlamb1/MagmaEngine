#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace Magma {

	struct ImageData {
		unsigned char* pixels = nullptr; 
		int32_t width = 0, height = 0, channels = 0;
		virtual ~ImageData() = default; 
	};

	class ImageLoader {

	public:
		virtual ~ImageLoader() = default; 
		virtual std::shared_ptr<ImageData> loadImage(const std::string& path) = 0; 

	};

}
