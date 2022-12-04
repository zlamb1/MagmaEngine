#include "vulkan_device.h"

namespace Magma {

    VulkanDevice::VulkanDevice(std::shared_ptr<VulkanInstance> pVulkanInstance,
        std::shared_ptr<VulkanSurface> pVulkanSurface, std::shared_ptr<VulkanValidater> pVulkanValidater) :
        pVulkanInstance{ pVulkanInstance }, pVulkanSurface{ pVulkanSurface },
        pVulkanValidater{ pVulkanValidater } {}

    VulkanDevice::~VulkanDevice() {
        if (allocator != nullptr)
            delete allocator;
        vkDestroyDevice(device, pAllocator);
    }

    VkResult VulkanDevice::init() {
        if (pVulkanInstance == nullptr) {
            Z_LOG_TXT("VulkanDevice::init", "pInstance is nullptr");
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        if (pVulkanSurface == nullptr) {
            Z_LOG_TXT("VulkanDevice::init", "pVulkanSurface is nullptr");
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        if (pVulkanValidater == nullptr) {
            Z_LOG_TXT("VulkanDevice::init", "pVulkanValidater is nullptr");
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        physicalDevice = VK_NULL_HANDLE;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(pVulkanInstance->getInstance(), &deviceCount, nullptr);

        if (deviceCount == 0) {
            Z_LOG_TXT("VulkanDevice::init", "could not find a GPU with Vulkan support!");
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(pVulkanInstance->getInstance(), &deviceCount, devices.data());

        auto& deviceProfile = DeviceProfile::instance();
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
            Z_LOG_TXT("VulkanDevice::init", "could not find a GPU that is suitable!");
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
        Z_LOG_OBJ("VulkanDevice::init::vkCreateDevice", createDeviceResult);

        if (createDeviceResult != VK_SUCCESS) {
            return createDeviceResult;
        }

        deviceProfile.setDevice(device);

        // TODO: make conditional
        allocator = new MemoryAllocator(physicalDevice, device, pVulkanInstance->getInstance());

        return VK_SUCCESS;
    }

    VkPhysicalDevice& VulkanDevice::getPhysicalDevice() {
        return physicalDevice;
    }

    VkDevice& VulkanDevice::getDevice() {
        return device;
    }

    const MemoryAllocator* VulkanDevice::getMemoryAllocator() const {
        return allocator; 
    }

    SwapchainSupportDetails VulkanDevice::querySwapchainSupport() {
        return DeviceProfile::querySwapchainSupport(physicalDevice, pVulkanSurface->getSurfaceKHR());
    }

}