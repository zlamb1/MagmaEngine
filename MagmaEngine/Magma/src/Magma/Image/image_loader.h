#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace Magma {

	struct ImageData {
		// specify deleter function on creation
		std::unique_ptr<char*> pixels; 
		int32_t width, height, channels; 
	};

	class ImageLoader {

	public:
		virtual ~ImageLoader() = default; 
		virtual ImageData loadImage(const std::string& path) = 0; 

	};

}
