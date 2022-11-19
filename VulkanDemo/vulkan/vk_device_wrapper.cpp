#include "vk_device_wrapper.h"

namespace VkDeviceUtility {

    static SwapChainSupportDetails querySwapchainSupport(
        VkPhysicalDevice& device, VkSurfaceKHR& surface) {

        SwapChainSupportDetails details;

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
                family.graphics = i;
                family.presentation = i;
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

        if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            return false;
        }

        if (!VkDeviceUtility::findQueueFamily(
            device, vkSurfaceKHR, VK_QUEUE_GRAPHICS_BIT).isComplete()) {
            return false;
        }

        bool extensionsSupported = VkDeviceUtility::checkDeviceExtensionsSupport(
            device, vkReqExtensions);
        if (!extensionsSupported) {
            return false;
        }

        SwapChainSupportDetails swapChainSupport = VkDeviceUtility::querySwapchainSupport(
            device, vkSurfaceKHR);
        if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
            return false;
        }

        return true;

    }

}

// Public

VkDeviceWrapper::~VkDeviceWrapper() {
    vkDestroyDevice(vkDevice, nullptr);
}

VkResult VkDeviceWrapper::create() {

    _VkLogger& logger = _VkLogger::Instance();

    vkPhysicalDevice = VK_NULL_HANDLE;

    if (pInstance == nullptr || pSurfaceKHR == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*pInstance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        logger.LogText("Could not find a GPU with Vulkan support!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(*pInstance, &deviceCount, devices.data());

    for (auto& device : devices) {
        if (VkDeviceUtility::isDeviceSuitable(device, *pSurfaceKHR, vkReqExtensions)) {
            vkPhysicalDevice = device;
            break;
        }
    }

    if (vkPhysicalDevice == VK_NULL_HANDLE) {
        logger.LogText("Could not find a GPU that is suitable!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Create Logical Device

    vkQueueFamily = VkDeviceUtility::findQueueFamily(
        vkPhysicalDevice, *pSurfaceKHR, VK_QUEUE_GRAPHICS_BIT);

    VkDeviceQueueCreateInfo queueCreateInfo{};

    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = vkQueueFamily.getGraphics();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(
        vkReqExtensions.size());
    createInfo.ppEnabledExtensionNames = vkReqExtensions.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    if (pValidationWrapper->enabled) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(pValidationWrapper->size());
        createInfo.ppEnabledLayerNames = pValidationWrapper->data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    auto vkCreateDeviceResult = vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &vkDevice);
    logger.LogResult("vkCreateDeviceResult =>", vkCreateDeviceResult);
    if (vkCreateDeviceResult != VK_SUCCESS) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    vkGetDeviceQueue(vkDevice, vkQueueFamily.getGraphics(), 0,
        &vkGraphicsQueue);
    vkGetDeviceQueue(vkDevice, vkQueueFamily.getPresentation(), 0,
        &vkPresentationQueue);

    return VK_SUCCESS;
}

SwapChainSupportDetails VkDeviceWrapper::querySwapChainSupport() {
    return VkDeviceUtility::querySwapchainSupport(vkPhysicalDevice, *pSurfaceKHR);
}