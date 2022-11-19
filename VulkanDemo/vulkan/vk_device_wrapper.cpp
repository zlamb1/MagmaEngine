#include "vk_device_wrapper.h"

// Public

VkDeviceWrapper::VkDeviceWrapper(VkInstance& _vkInstance,
    VkSurfaceWrapper& _vkSurfaceWrapper,
    VkValidationWrapper& _vkValidationWrapper) 
    : VkDeviceWrapper(_vkInstance, _vkSurfaceWrapper, _vkValidationWrapper, false) {

}

VkDeviceWrapper::VkDeviceWrapper(VkInstance& _vkInstance, 
    VkSurfaceWrapper& _vkSurfaceWrapper,
    VkValidationWrapper& _vkValidationWrapper, 
    bool _debug) 
    : vkInstance{ _vkInstance }, vkSurfaceWrapper{ _vkSurfaceWrapper },
    vkValidationWrapper{ _vkValidationWrapper }, debug{ _debug } {
	selectPhysicalDevice();
    createLogicalDevice();
}

VkDeviceWrapper::~VkDeviceWrapper() {
    vkDestroyDevice(vkLogicalDevice, nullptr);
}

VkPhysicalDevice& VkDeviceWrapper::getPhysicalDevice() {
    return vkPhysicalDevice;
}

VkDevice& VkDeviceWrapper::getLogicalDevice() {
    return vkLogicalDevice;
}

QueueFamily& VkDeviceWrapper::getQueueFamily() {
    return family;
}

VkQueue& VkDeviceWrapper::getGraphicsQueue() {
    return vkGraphicsQueue;
}

VkQueue& VkDeviceWrapper::getPresentationQueue() {
    return vkPresentQueue;
}

QueueFamily VkDeviceWrapper::findQueueFamily(VkQueueFlagBits flags) {
    return findQueueFamily(vkPhysicalDevice, vkSurfaceWrapper.getSurface(), flags);
}

SwapChainSupportDetails VkDeviceWrapper::querySwapChainSupport() {
    return querySwapChainSupport(vkPhysicalDevice, vkSurfaceWrapper.getSurface());
}

// Private

void VkDeviceWrapper::selectPhysicalDevice() {
    vkPhysicalDevice = VK_NULL_HANDLE;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            vkPhysicalDevice = device;
            break;
        }
    }

    if (vkPhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
    else if (debug) {
        std::cout << "Device found: " << vkPhysicalDevice << std::endl;
        auto family = findQueueFamily(VK_QUEUE_GRAPHICS_BIT);
        std::cout << "\tGraphics queue index: " << family.getGraphics() << std::endl;
        std::cout << "\tPresentation queue index: " << family.getPresent() << std::endl << std::endl;
    }
}

bool VkDeviceWrapper::isDeviceSuitable(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        return false;
    }

    if (!findQueueFamily(device,
        vkSurfaceWrapper.getSurface(),
        VK_QUEUE_GRAPHICS_BIT).isComplete()) {
        return false;
    }

    bool extensionsSupported = checkDeviceExtensionsSupport(device);
    if (!extensionsSupported) {
        return false;
    }

    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(
        device, vkSurfaceWrapper.getSurface());
    if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
        return false;
    }

    return true;
}

bool VkDeviceWrapper::checkDeviceExtensionsSupport(VkPhysicalDevice& device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(
        requiredDeviceExtensions.begin(), 
        requiredDeviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

void VkDeviceWrapper::createLogicalDevice() {
    family = findQueueFamily(VK_QUEUE_GRAPHICS_BIT);

    VkDeviceQueueCreateInfo queueCreateInfo{};

    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = family.getGraphics();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(
        requiredDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    if (vkValidationWrapper.enabled) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(vkValidationWrapper.size());
        createInfo.ppEnabledLayerNames = vkValidationWrapper.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &vkLogicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
    else {
        std::cout << "Queue creation:" << std::endl;
        vkGetDeviceQueue(vkLogicalDevice, family.getGraphics(), 0, 
            &vkGraphicsQueue);
        std::cout << "\tCreated graphics queue" << std::endl;
        vkGetDeviceQueue(vkLogicalDevice, family.getPresent(), 0, 
            &vkPresentQueue);
        std::cout << "\tCreated present queue" << std::endl << std::endl;
    }
}

// Static

QueueFamily VkDeviceWrapper::findQueueFamily(
    VkPhysicalDevice& device, 
    VkSurfaceKHR& surface, 
    VkQueueFlagBits flags) {
    QueueFamily family;

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
            family.present = i;
        }
        i++;
    }

    return family;
}

SwapChainSupportDetails VkDeviceWrapper::querySwapChainSupport(
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