#include "vulkan_instance.h"

namespace Magma {

    VulkanInstance::VulkanInstance(std::shared_ptr<VulkanValidater> pVulkanValidater) :
        pVulkanValidater{ pVulkanValidater } {}

    VulkanInstance::~VulkanInstance() {
        vkDestroyInstance(vkInstance, pAllocator);
    }

    VkResult VulkanInstance::init() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "VulkanDemo";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;

        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(pRequiredExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = pRequiredExtensions.data();

        if (pVulkanValidater->isValidationEnabled()) {
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(pVulkanValidater->getLayerSize());
            instanceCreateInfo.ppEnabledLayerNames = pVulkanValidater->getLayerData();
            instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&pDebugCreateInfo;
        }
        else {
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.pNext = nullptr;
        }

        auto createInstanceResult = vkCreateInstance(&instanceCreateInfo, pAllocator, &vkInstance);
        switch (createInstanceResult) {
        case VK_ERROR_LAYER_NOT_PRESENT:
            Z_LOG_TXT("VulkanAPI::initInstance::vkCreateInstance", "failed to find required layers");
            Z_LOG_TXT("VulkanAPI::initInstance", "Trying vkCreateInstance again...");

            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.ppEnabledLayerNames = nullptr;

            // try to create instance again with no layers
            createInstanceResult = vkCreateInstance(&instanceCreateInfo, pAllocator, &vkInstance);
            Z_LOG_OBJ("VulkanAPI::initInstance::vkCreateInstance", createInstanceResult);
            break;
        case VK_SUCCESS:
            break;
        default:
            Z_LOG_OBJ("VulkanAPI::initInstance::vkCreateInstance", createInstanceResult);
            break;
        }

        return VK_SUCCESS;
    }

    VkInstance& VulkanInstance::getInstance() {
        return vkInstance;
    }

}