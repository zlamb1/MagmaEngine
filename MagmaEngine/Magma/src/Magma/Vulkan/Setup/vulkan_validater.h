#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace Magma {
	class VulkanValidater {

	public:
		VulkanValidater() = default;

		bool isValidationEnabled();
		void setValidationEnabled(bool validationEnabled);

		size_t getLayerSize();
		const char* const* getLayerData();

		bool ensureRequiredLayers();

	private:
		bool validationEnabled = true;

		const std::vector<const char*> enabledValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

	};
}
