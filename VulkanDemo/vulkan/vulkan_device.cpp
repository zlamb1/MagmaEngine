#include "vulkan_device.h"

namespace DeviceUtility {
    static SwapchainSupportDetails querySwapchainSupport(
        VkPhysicalDevice& device, VkSurfaceKHR& surface) {

        SwapchainSupportDetails details;

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

    static QueueFamily findQueueFamily(VkPhysicalDevice& device, VkSurfaceKHR& surface,
        VkQueueFlagBits flags) {

        QueueFamily family{};

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (family.isComplete()) {
                return family;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(
                device, i,
                surface, &presentSupport);

            if (queueFamily.queueFlags & flags && presentSupport) {
                family.setGraphics(i);
                family.setPresentation(i);
            }

            i++;
        }

        return family;
    }

    static bool checkDeviceExtensionsSupport(VkPhysicalDevice& device,
        std::vector<const char*> vkReqExtensions) {

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(vkReqExtensions.begin(), vkReqExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    static bool isDeviceSuitable(VkPhysicalDevice& device, VkSurfaceKHR& vkSurfaceKHR,
        std::vector<const char*> vkReqExtensions) {

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        if (!DeviceUtility::findQueueFamily(
            device, vkSurfaceKHR, VK_QUEUE_GRAPHICS_BIT).isComplete()) {
            return false;
        }

        bool extensionsSupported = DeviceUtility::checkDeviceExtensionsSupport(
            device, vkReqExtensions);
        if (!extensionsSupported) {
            return false;
        }

        SwapchainSupportDetails swapchainSupport = DeviceUtility::querySwapchainSupport(
            device, vkSurfaceKHR);
        if (swapchainSupport.formats.empty() || swapchainSupport.presentModes.empty()) {
            return false;
        }

        return true;
    }
}

// QueueFamily

uint32_t QueueFamily::getGraphics() {
    return graphics.value();
}

uint32_t QueueFamily::getPresentation() {
    return presentation.value();
}

void QueueFamily::setGraphics(uint32_t graphics) {
    this->graphics = graphics; 
}

void QueueFamily::setPresentation(uint32_t presentation) {
    this->presentation = presentation;
}

bool QueueFamily::isComplete() {
    return graphics.has_value() && presentation.has_value(); 
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
        VulkanLogger::instance().enqueueText("VulkanDevice", "Could not find a GPU with Vulkan support!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(pVulkanInstance->getInstance(), &deviceCount, devices.data());

    for (auto& device : devices) {
        if (DeviceUtility::isDeviceSuitable(device, pVulkanSurface->getSurfaceKHR(), reqExtensions)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        VulkanLogger::instance().enqueueText("VulkanDevice", "Could not find a GPU that is suitable!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Create Logical Device

    queueFamily = DeviceUtility::findQueueFamily(
        physicalDevice, pVulkanSurface->getSurfaceKHR(), VK_QUEUE_GRAPHICS_BIT);

    VkDeviceQueueCreateInfo queueCreateInfo{};

    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily.getGraphics();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(
        reqExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = reqExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    if (pVulkanValidater->isValidationEnabled()) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(pVulkanValidater->getLayerSize());
        deviceCreateInfo.ppEnabledLayerNames = pVulkanValidater->getLayerData();
    }
    else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    auto createDeviceResult = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
    VulkanLogger::instance().enqueueObject("vkCreateDeviceResult", createDeviceResult);
    if (createDeviceResult != VK_SUCCESS) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    vkGetDeviceQueue(device, queueFamily.getGraphics(), 0,
        &graphicsQueue);
    vkGetDeviceQueue(device, queueFamily.getPresentation(), 0,
        &presentationQueue);

    return VK_SUCCESS;
}

VkPhysicalDevice& VulkanDevice::getPhysicalDevice() {
    return physicalDevice;
}

VkDevice& VulkanDevice::getDevice() {
    return device;
}

QueueFamily& VulkanDevice::getQueueFamily() {
    return queueFamily;
}

VkQueue& VulkanDevice::getGraphicsQueue() {
    return graphicsQueue;
}

VkQueue& VulkanDevice::getPresentationQueue() {
    return presentationQueue;
}

SwapchainSupportDetails VulkanDevice::querySwapchainSupport() {
    return DeviceUtility::querySwapchainSupport(physicalDevice, pVulkanSurface->getSurfaceKHR());
}