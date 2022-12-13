#pragma once

#include <vulkan/vulkan.h>

#include "Magma/Render/render_enums.h"

namespace Magma {

	class Image {

	public:
		virtual ~Image() = default; 
		virtual VkResult init() = 0; 

	public:
		uint32_t m_ExtentWidth = 0, m_ExtentHeight = 0, m_ExtentDepth = 1;
		uint32_t m_MipLevels = 1, m_ArrayLayers = 1;

		ImageType m_Type = ImageType::IMAGE_TYPE_2D;
		DataFormat m_Format = DataFormat::R8G8B8A8_SRGB;
		ImageTiling m_Tiling = ImageTiling::OPTIMAL;
		ImageLayout m_InitialLayout = ImageLayout::UNDEFINED;
		SharingMode m_SharingMode = SharingMode::EXCLUSIVE;
		SampleCount m_Samples = SampleCount::SAMPLE_COUNT_1_BIT;
		uint32_t m_Flags = 0; 

	};

}
