#include "device_profile.h"

namespace Magma {

    DeviceProfile& DeviceProfile::instance() {
        static DeviceProfile instance;
        return instance;
    }

    VkPhysicalDevice& DeviceProfile::getDevice() {
        return vkPhysicalDevice;
    }

    VulkanDeviceType DeviceProfile::getDeviceType() {
        return vulkanDeviceType;
    }

    std::map<VulkanQueueType, std::optional<uint32_t>>& DeviceProfile::getQueueIndices() {
        return queueIndices;
    }

    std::optional<VkQueue> DeviceProfile::getQueue(VulkanQueueType vulkanQueueType) {
        return instance().getVkQueue(vulkanQueueType);
    }

    std::optional<VkQueue> DeviceProfile::getVkQueue(VulkanQueueType vulkanQueueType) {
        if (!queues[vulkanQueueType].has_value()) {
            auto queueIndex = queueIndices[vulkanQueueType];
            if (queueIndex.has_value()) {
                queues[vulkanQueueType] = std::make_optional<VkQueue>();
                vkGetDeviceQueue(vkDevice, queueIndex.value(), 0, &queues[vulkanQueueType].value());
            }
        }
        return queues[vulkanQueueType];
    }

    std::vector<const char*>& DeviceProfile::getRequiredExtensions() {
        return vulkanDeviceExtensions;
    }

    void DeviceProfile::generateProfile(VkPhysicalDevice vkPhysicalDevice) {
        if (pVulkanSurface == nullptr) {
            return;
        }

        this->vkPhysicalDevice = vkPhysicalDevice;

        for (auto& pair : deviceQueueTypes) {
            auto queueType = pair.first;
            auto priority = pair.second;
            if (priority.has_value()) {
                auto found = findQueueFamily(vkPhysicalDevice, pVulkanSurface->getSurfaceKHR(),
                    queueType, queueSupportsPresentation[queueType]);
                if (found.hasQueue()) {
                    queueIndices[queueType] = found.getQueue();
                }
            }
        }
    }

    void DeviceProfile::setDevice(VkDevice vkDevice) {
        this->vkDevice = vkDevice;
    }

    void DeviceProfile::setSurface(std::shared_ptr<Surface> surface) {
        this->pVulkanSurface = surface;
    }

    void DeviceProfile::setDeviceTypePriority(VulkanPriority vulkanDeviceTypePriority) {
        this->vulkanDeviceTypePriority = vulkanDeviceTypePriority;
    }

    void DeviceProfile::setDeviceType(VulkanDeviceType vulkanDeviceType) {
        this->vulkanDeviceType = vulkanDeviceType;
    }

    void DeviceProfile::setQueueType(VulkanQueueType vulkanQueueType,
        VulkanPriority queueTypePriority) {
        deviceQueueTypes[vulkanQueueType] = queueTypePriority;
    }

    void DeviceProfile::setQueueSupportsPresentation(VulkanQueueType vulkanQueueType, VkBool32 val) {
        queueSupportsPresentation[vulkanQueueType] = val;
    }

    int32_t DeviceProfile::getDeviceScore(VkPhysicalDevice& vkPhysicalDevice) {
        if (pVulkanSurface == nullptr) {
            return -1;
        }

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &deviceFeatures);

        int32_t score = 0;

        if (deviceProperties.deviceType != (VkPhysicalDeviceType)vulkanDeviceType
            && vulkanDeviceTypePriority == VulkanPriority::REQUIRED) {
            return -1;
        }

        if (deviceProperties.deviceType == (VkPhysicalDeviceType)vulkanDeviceType) {
            score += (int32_t)vulkanDeviceTypePriority;
        }

        for (auto& pair : deviceQueueTypes) {
            VkBool32 presentationRequired = queueSupportsPresentation[pair.first];
            auto found = findQueueFamily(vkPhysicalDevice, pVulkanSurface->getSurfaceKHR(),
                pair.first, presentationRequired);
            auto priority = pair.second;
            if (found.hasQueue()) {
                if (presentationRequired && !found.hasPresentation()) {
                    return -1;
                }
                if (priority != VulkanPriority::REQUIRED)
                    score += (uint32_t)pair.second.value();
            }
            else if (priority == VulkanPriority::REQUIRED) {
                return -1;
            }
        }

        if (!DeviceProfile::checkDeviceExtensionsSupport(vkPhysicalDevice, vulkanDeviceExtensions)) {
            return -1;
        }

        SwapchainSupportDetails swapchainSupport = DeviceProfile::querySwapchainSupport(
            vkPhysicalDevice, pVulkanSurface->getSurfaceKHR());
        if (swapchainSupport.formats.empty() || swapchainSupport.presentModes.empty()) {
            return -1;
        }

        return score;
    }

    QueueFamily DeviceProfile::findQueueFamily(VkPhysicalDevice& vkPhysicalDevice,
        VkSurfaceKHR& vkSurfaceKHR, VulkanQueueType vulkanQueueType, VkBool32 mustSupportPresentation) {
        QueueFamily family{};

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

        for (int i = 0; i < queueFamilies.size(); i++) {
            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, vkSurfaceKHR, &presentSupport);

            if (queueFamilies[i].queueFlags & (VkQueueFlagBits)vulkanQueueType) {
                family.setQueue(i);
                if (!mustSupportPresentation) {
                    return family;
                }
                else if (presentSupport) {
                    family.setPresentation(i);
                    return family;
                }
            }
        }

        family.setQueue();
        family.setPresentation();
        return family;
    }

    SwapchainSupportDetails DeviceProfile::querySwapchainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface) {
        SwapchainSupportDetails details{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    bool DeviceProfile::checkDeviceExtensionsSupport(VkPhysicalDevice& device,
        std::vector<const char*> requiredExtensions) {

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> extensions(requiredExtensions.begin(), requiredExtensions.end());

        for (const auto& extension : availableExtensions) {
            extensions.erase(extension.extensionName);
        }

        return extensions.empty();
    }

}