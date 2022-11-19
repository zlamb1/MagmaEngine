#include "vk_validation_wrapper.h"

size_t _VkValidation::size() {
	return vkValidationLayers.size();
}

const char* const* _VkValidation::data() {
	return vkValidationLayers.data();
}

bool _VkValidation::ensureRequiredLayers() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : vkValidationLayers) {
        for (int i = 0; i < availableLayers.size(); i++) {
            if (strcmp(layerName, availableLayers[i].layerName) == 0) {
                break;
            }
            else if (i == availableLayers.size() - 1) {
                return false;
            }
        }
    }

    return true;
}