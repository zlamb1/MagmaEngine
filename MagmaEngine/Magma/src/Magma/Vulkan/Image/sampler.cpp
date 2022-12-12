#include "sampler.h"

namespace Magma {

	Sampler::~Sampler() {
		if (pDevice != nullptr) {
			vkDestroySampler(pDevice->getDevice(), sampler, nullptr);
		}
	}

	VkResult Sampler::init() {
		if (pDevice == nullptr) {
			Z_LOG_TXT("Sampler::init", "pDevice is nullptr");
			return VK_ERROR_INITIALIZATION_FAILED;
		}
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = pMagFilter;
		samplerInfo.minFilter = pMinFilter;
		samplerInfo.addressModeU = uAddressMode;
		samplerInfo.addressModeV = vAddressMode;
		samplerInfo.addressModeW = wAddressMode;
		samplerInfo.anisotropyEnable = pAnisotropyEnable;
		if (pAnisotropyEnable == VK_TRUE)
			samplerInfo.maxAnisotropy = getMaxAnisotropy();
		else samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.borderColor = pBorderColor;
		samplerInfo.unnormalizedCoordinates = pUnnormalizedCoordinates;
		samplerInfo.compareEnable = pCompareEnable;
		samplerInfo.compareOp = pCompareOp;
		samplerInfo.mipmapMode = pMipmapMode;
		samplerInfo.mipLodBias = pMipLodBias;
		samplerInfo.minLod = pMinLod;
		samplerInfo.maxLod = pMaxLod;
		return vkCreateSampler(pDevice->getDevice(), &samplerInfo, nullptr, &sampler);
	}

	const VkSampler& Sampler::getSampler() const {
		return sampler;
	}

	float Sampler::getMaxAnisotropy() {
		auto& deviceProfile = DeviceProfile::instance();
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(deviceProfile.getDevice(), &properties);
		return properties.limits.maxSamplerAnisotropy;
	}

}