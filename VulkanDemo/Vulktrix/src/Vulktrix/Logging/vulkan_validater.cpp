#include "vulkan_validater.h"

bool VulkanValidater::isValidationEnabled() {
    return validationEnabled;
}

void VulkanValidater::setValidationEnabled(bool validationEnabled) {
    this->validationEnabled = validationEnabled;
}

size_t VulkanValidater::getLayerSize() {
	return enabledValidationLayers.size();
}

const char* const* VulkanValidater::getLayerData() {
	return enabledValidationLayers.data();
}

bool VulkanValidater::ensureRequiredLayers() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : enabledValidationLayers) {
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