#include "vulkan_device.h"

namespace DeviceUtility {
    static SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface) {
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

    static bool checkDeviceExtensionsSupport(VkPhysicalDevice& device,
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

// QueueFamily

uint32_t QueueFamily::getQueue() {
    return queue.value();
}

uint32_t QueueFamily::getPresentation() {
    return presentation.value();
}

void QueueFamily::setQueue() {
    this->presentation = {};
}

void QueueFamily::setQueue(uint32_t queue) {
    this->queue = queue;
}

void QueueFamily::setPresentation() {
    this->presentation = {};
}

void QueueFamily::setPresentation(uint32_t presentation) {
    this->presentation = presentation;
}

bool QueueFamily::hasQueue() {
    return queue.has_value(); 
}

bool QueueFamily::hasPresentation() {
    return presentation.has_value();
}

// VulkanDevice

VulkanDevice::VulkanDevice(std::shared_ptr<VulkanInstance> pVulkanInstance, 
    std::shared_ptr<VulkanSurface> pVulkanSurface, std::shared_ptr<VulkanValidater> pVulkanValidater) :
    pVulkanInstance{ pVulkanInstance }, pVulkanSurface{ pVulkanSurface }, 
    pVulkanValidater{ pVulkanValidater } {}

VulkanDevice::~VulkanDevice() {
    vkDestroyDevice(device, pAllocator);
}

VkResult VulkanDevice::init() {
    if (pVulkanInstance == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanDevice::init", "pInstance is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pVulkanSurface == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanDevice::init", "pVulkanSurface is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pVulkanValidater == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanDevice::init", "pVulkanValidater is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    physicalDevice = VK_NULL_HANDLE;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(pVulkanInstance->getInstance(), &deviceCount, nullptr);

    if (deviceCount == 0) {
        VulkanLogger::instance().enqueueText("VulkanDevice::init", "could not find a GPU with Vulkan support!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(pVulkanInstance->getInstance(), &deviceCount, devices.data());

    auto& deviceProfile = VulkanDeviceProfile::instance();
    deviceProfile.setVulkanSurface(pVulkanSurface);

    int32_t bScore = -1; 
    for (auto& device : devices) {
        auto deviceScore = deviceProfile.getDeviceScore(device);
        if (deviceScore > bScore) {
            bScore = deviceScore;
            physicalDevice = device; 
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        VulkanLogger::instance().enqueueText("VulkanDevice::init", "could not find a GPU that is suitable!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // generate queue index map
    deviceProfile.generateProfile(physicalDevice);

    // create queue create info structs
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    for (auto& queueIndex : deviceProfile.getQueueIndices()) {
        auto index = queueIndex.second;
        if (index.has_value()) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = index.value();
            queueCreateInfo.queueCount = 1;

            float queuePriority = 1.0f;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }
    }

    // create logical device
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(
        deviceProfile.getRequiredExtensions().size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceProfile.getRequiredExtensions().data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    if (pVulkanValidater->isValidationEnabled()) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(pVulkanValidater->getLayerSize());
        deviceCreateInfo.ppEnabledLayerNames = pVulkanValidater->getLayerData();
    }
    else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    auto createDeviceResult = vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, &device);
    VulkanLogger::instance().enqueueObject("VulkanDevice::init::vkCreateDevice", createDeviceResult);
    if (createDeviceResult != VK_SUCCESS) {
        return createDeviceResult;
    }

    deviceProfile.setDevice(device);

    return VK_SUCCESS;
}

VkPhysicalDevice& VulkanDevice::getPhysicalDevice() {
    return physicalDevice;
}

VkDevice& VulkanDevice::getDevice() {
    return device;
}

SwapchainSupportDetails VulkanDevice::querySwapchainSupport() {
    return DeviceUtility::querySwapchainSupport(physicalDevice, pVulkanSurface->getSurfaceKHR());
}

// VulkanDeviceProfile

VulkanDeviceProfile& VulkanDeviceProfile::instance() {
    static VulkanDeviceProfile instance;
    return instance;
}

VkPhysicalDevice& VulkanDeviceProfile::getDevice() {
    return vkPhysicalDevice; 
}

VulkanDeviceType VulkanDeviceProfile::getDeviceType() {
    return vulkanDeviceType; 
}

std::map<VulkanQueueType, std::optional<uint32_t>>& VulkanDeviceProfile::getQueueIndices() {
    return queueIndices;
}

std::optional<VkQueue> VulkanDeviceProfile::getQueue(VulkanQueueType vulkanQueueType) {
    if (!queues[vulkanQueueType].has_value()) {
        auto queueIndex = queueIndices[vulkanQueueType];
        if (queueIndex.has_value()) {
            queues[vulkanQueueType] = std::make_optional<VkQueue>();
            vkGetDeviceQueue(vkDevice, queueIndex.value(), 0, &queues[vulkanQueueType].value()) ;
        }
    }
    return queues[vulkanQueueType];
}

std::vector<const char*>& VulkanDeviceProfile::getRequiredExtensions() {
    return vulkanDeviceExtensions;
}

void VulkanDeviceProfile::generateProfile(VkPhysicalDevice vkPhysicalDevice) {
    if (pVulkanSurface == nullptr) {
        return;
    }

    this->vkPhysicalDevice = vkPhysicalDevice;

    for (auto& pair : deviceQueueTypes) {
        auto queueType = pair.first; 
        auto priority = pair.second;
        if (priority.has_value()) {
            QueueFamily found = findQueueFamily(vkPhysicalDevice, pVulkanSurface->getSurfaceKHR(),
                queueType, queueSupportsPresentation[queueType]);
            if (found.hasQueue()) {
                queueIndices[queueType] = found.getQueue();
            }
        }
    }
}

void VulkanDeviceProfile::setDevice(VkDevice vkDevice) {
    this->vkDevice = vkDevice;
}

void VulkanDeviceProfile::setVulkanSurface(std::shared_ptr<VulkanSurface> pVulkanSurface) {
    this->pVulkanSurface = pVulkanSurface;
}

void VulkanDeviceProfile::setDeviceTypePriority(VulkanPriority vulkanDeviceTypePriority) {
    this->vulkanDeviceTypePriority = vulkanDeviceTypePriority;
}

void VulkanDeviceProfile::setDeviceType(VulkanDeviceType vulkanDeviceType) {
    this->vulkanDeviceType = vulkanDeviceType;
}

void VulkanDeviceProfile::setQueueType(VulkanQueueType vulkanQueueType, 
    VulkanPriority queueTypePriority) {
    deviceQueueTypes[vulkanQueueType] = queueTypePriority;
}

void VulkanDeviceProfile::setQueueSupportsPresentation(VulkanQueueType vulkanQueueType, VkBool32 val) {
    queueSupportsPresentation[vulkanQueueType] = val;
}

int32_t VulkanDeviceProfile::getDeviceScore(VkPhysicalDevice& vkPhysicalDevice) {
    if (pVulkanSurface == nullptr) {
        return -1; 
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &deviceFeatures);

    int32_t score = 0;

    if (deviceProperties.deviceType != (VkPhysicalDeviceType) vulkanDeviceType 
        && vulkanDeviceTypePriority == VulkanPriority::REQUIRED) {
        return -1; 
    }

    if (deviceProperties.deviceType == (VkPhysicalDeviceType) vulkanDeviceType) {
        score += (int32_t) vulkanDeviceTypePriority; 
    }

    for (auto& pair : deviceQueueTypes) {
        VkBool32 presentationRequired = queueSupportsPresentation[pair.first];
        QueueFamily found = findQueueFamily(vkPhysicalDevice, pVulkanSurface->getSurfaceKHR(), 
            pair.first, presentationRequired);
        auto priority = pair.second;
        if (found.hasQueue()) {
            if (presentationRequired && !found.hasPresentation()) {
                return -1; 
            }
            if (priority != VulkanPriority::REQUIRED)
                score += (uint32_t) pair.second.value();
        }
        else if (priority == VulkanPriority::REQUIRED) {
            return -1; 
        }
    }

    if (!DeviceUtility::checkDeviceExtensionsSupport(vkPhysicalDevice, vulkanDeviceExtensions)) {
        return -1; 
    }

    SwapchainSupportDetails swapchainSupport = DeviceUtility::querySwapchainSupport(
        vkPhysicalDevice, pVulkanSurface->getSurfaceKHR());
    if (swapchainSupport.formats.empty() || swapchainSupport.presentModes.empty()) {
        return -1; 
    }

    return score;
}

QueueFamily VulkanDeviceProfile::findQueueFamily(VkPhysicalDevice& vkPhysicalDevice, 
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