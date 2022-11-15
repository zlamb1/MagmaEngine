#pragma once

#include <vector>

#include <vulkan/vulkan.h>

class VkValidationWrapper {

	public:

		bool enabled = true; 

		size_t size();
		const char* const* data();

		bool ensureRequiredLayers();

	private:

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

};