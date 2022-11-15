#include "vk_validation_wrapper.h"

size_t VkValidationWrapper::size() {
	return validationLayers.size();
}

const char* const* VkValidationWrapper::data() {
	return validationLayers.data();
}

bool VkValidationWrapper::ensureRequiredLayers() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
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