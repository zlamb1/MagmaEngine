#pragma once

#include <vector>

#include <vulkan/vulkan.h>

class _VkValidation {

public:
	bool validationEnabled = true; 

	size_t size();
	const char* const* data();

	bool ensureRequiredLayers();

private:
	const std::vector<const char*> vkValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

};