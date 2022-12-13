#include "validater.h"

#include "instance.h"

namespace Magma {

    bool Validater::isValidationEnabled() const {
        return m_ValidationEnabled;
    }

    void Validater::setValidationEnabled(bool validationEnabled) {
        m_ValidationEnabled = validationEnabled;
    }

    size_t Validater::getLayerSize() const {
        return m_ValidationLayers.size();
    }

    const char* const* Validater::getLayerData() const {
        return m_ValidationLayers.data();
    }

    bool Validater::ensureRequiredLayers() const {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        for (const char* layerName : m_ValidationLayers) {
            const int numberOfLayers = static_cast<int>(availableLayers.size());
            for (int i = 0; i < numberOfLayers; i++) {
                if (strcmp(layerName, availableLayers[i].layerName) == 0) {
                    break;
                }
                else if (i == numberOfLayers - 1) {
                    return false;
                }
            }
        }
        return true;
    }

}
