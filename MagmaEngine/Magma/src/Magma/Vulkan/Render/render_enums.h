#pragma once

#include <stdexcept>

#include <vulkan/vulkan.h>

#include "Magma/Render/render_enums.h"

namespace Magma {

	inline VkVertexInputRate convertVertexInputRate(VertexInputRate inputRate) {
		switch (inputRate) {
			case VertexInputRate::VERTEX: return VK_VERTEX_INPUT_RATE_VERTEX;
			case VertexInputRate::INSTANCE: return VK_VERTEX_INPUT_RATE_INSTANCE; 
			default:
				throw std::invalid_argument("couldn't convert VertexInputRate[" + 
					std::to_string(static_cast<int>(inputRate)) + "] to VkVertexInputRate");
		}
	}

	inline VkImageType convertImageType(const ImageType type) {
		switch (type) {
			case ImageType::IMAGE_TYPE_1D: return VK_IMAGE_TYPE_1D;
			case ImageType::IMAGE_TYPE_2D: return VK_IMAGE_TYPE_2D;
			case ImageType::IMAGE_TYPE_3D: return VK_IMAGE_TYPE_3D;
			default: throw std::invalid_argument("couldn't convert ImageType[" + 
				std::to_string(static_cast<int>(type)) + "] to VkImageType!");
		}
	}

	inline VkImageTiling convertImageTiling(const ImageTiling tiling) {
		switch (tiling) {
			case ImageTiling::DRM_FORMAT_MODIFIER_EXT: return VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT;
			case ImageTiling::LINEAR: return VK_IMAGE_TILING_LINEAR;
			case ImageTiling::OPTIMAL: return VK_IMAGE_TILING_OPTIMAL;
			default: throw std::invalid_argument("couldn't convert ImageTiling[" +
				std::to_string(static_cast<int>(tiling)) + "] to VkImageTiling"); 
		}
	}

	inline VkImageLayout convertImageLayout(const ImageLayout layout) {
		switch (layout) {
			case ImageLayout::UNDEFINED: return VK_IMAGE_LAYOUT_UNDEFINED;
			case ImageLayout::TRANSFER_DST_OPTIMAL: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			case ImageLayout::SHADER_READ_ONLY_OPTIMAL: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; 
			default: throw std::invalid_argument("couldn't convert ImageLayout[" +
				std::to_string(static_cast<int>(layout)) + "] to VkImageLayout");
		}
	}

	inline VkImageUsageFlagBits convertImageUsageFlags(const ImageUsage usage) {
		switch (usage) {
			case ImageUsage::SAMPLED_BIT: return VK_IMAGE_USAGE_SAMPLED_BIT;
			case ImageUsage::TRANSFER_SRC_BIT: return VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			case ImageUsage::TRANSFER_DST_BIT: return VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			case ImageUsage::DEPTH_STENCIAL_ATTACHMENT_BIT: return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			default: throw std::invalid_argument("couldn't convert ImageUsage[" + 
				std::to_string(static_cast<int>(usage)) + "] to VkImageUsageFlagBits!");
		}
	}

	inline VkSharingMode convertSharingMode(const SharingMode sharingMode) {
		switch (sharingMode) {
			case SharingMode::EXCLUSIVE: return VK_SHARING_MODE_EXCLUSIVE;
			case SharingMode::CONCURRENT: return VK_SHARING_MODE_CONCURRENT;
			default: throw std::invalid_argument("couldn't convert SharingMode[" + 
				std::to_string(static_cast<int>(sharingMode)) + "] to VkSharingMode!");
		}
	}

	inline VkSampleCountFlagBits convertSampleCount(const SampleCount sampleCount) {
		switch (sampleCount) {
			case SampleCount::SAMPLE_COUNT_1_BIT: return VK_SAMPLE_COUNT_1_BIT;
			case SampleCount::SAMPLE_COUNT_2_BIT: return VK_SAMPLE_COUNT_2_BIT;
			case SampleCount::SAMPLE_COUNT_4_BIT: return VK_SAMPLE_COUNT_4_BIT;
			case SampleCount::SAMPLE_COUNT_8_BIT: return VK_SAMPLE_COUNT_8_BIT;
			case SampleCount::SAMPLE_COUNT_16_BIT: return VK_SAMPLE_COUNT_16_BIT;
			case SampleCount::SAMPLE_COUNT_32_BIT: return VK_SAMPLE_COUNT_32_BIT;
			case SampleCount::SAMPLE_COUNT_64_BIT: return VK_SAMPLE_COUNT_64_BIT;
			default: throw std::invalid_argument("couldn't convert SampleCount[" + 
				std::to_string(static_cast<int>(sampleCount)) + "] to VkSampleCountFlagBits!");
		}
	}

}