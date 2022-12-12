#pragma once

#include "Magma/Vulkan/Device/vulkan_device.h"

namespace Magma {

	class Sampler : VulkanObject {

	public:
		Sampler() = default; 
		~Sampler() override;

		VkResult init() override;

		const VkSampler& getSampler() const;

	public:
		std::shared_ptr<VulkanDevice> pDevice = nullptr;

		VkFilter pMagFilter = VK_FILTER_LINEAR, pMinFilter = VK_FILTER_LINEAR;
		VkSamplerAddressMode uAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			vAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			wAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkBool32 pAnisotropyEnable = VK_TRUE;
		VkBorderColor pBorderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		VkBool32 pUnnormalizedCoordinates = VK_FALSE, pCompareEnable = VK_FALSE;
		VkCompareOp pCompareOp = VK_COMPARE_OP_ALWAYS;
		VkSamplerMipmapMode pMipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		float pMipLodBias = 0.0f, pMinLod = 0.0f, pMaxLod = 0.0f;

	private:
		float getMaxAnisotropy();

	private:
		VkSampler sampler{};

	};

}

