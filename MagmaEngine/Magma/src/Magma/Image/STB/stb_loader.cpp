#define STB_IMAGE_IMPLEMENTATION
#include "Magma/Image/STB/stb_image.h"

#include "stb_loader.h"

namespace Magma {

	StbImageData::~StbImageData() {
		stbi_image_free(pixels);
	}

	StbImageLoader::StbImageLoader() {
		// set vertically flip for Vulkan
		// possibly abstract this based on which graphics API is being used
		stbi_set_flip_vertically_on_load(true);
	}

	std::shared_ptr<ImageData> StbImageLoader::loadImage(const std::string& path) {
		std::shared_ptr<ImageData> imageData = std::make_shared<StbImageData>();
		imageData->pixels = stbi_load(path.c_str(), &imageData->width, &imageData->height,
			&imageData->channels, STBI_rgb_alpha);
		return imageData; 
	}

}