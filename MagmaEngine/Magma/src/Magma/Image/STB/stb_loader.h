#pragma once

#include "Magma/Image/image_loader.h"

namespace Magma {

	struct StbImageData : ImageData {
		~StbImageData() override; 
	};

	class StbImageLoader : public ImageLoader {

	public:
		StbImageLoader(); 

		std::shared_ptr<ImageData> loadImage(const std::string& path) override; 

	};

}
