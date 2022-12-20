#include "instance.h"

#include "Magma/Vulkan/Logging/vulkan_logger.h"

namespace Magma {

    Instance::Instance(std::shared_ptr<Validater> validater) :
        m_Validater{ std::move(validater) } {}

    Instance::~Instance() {
        vkDestroyInstance(m_Instance, pAllocator);
    }

    VkResult Instance::init() {
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

        if (m_RequiredExtensions.empty()) {
            instanceCreateInfo.enabledExtensionCount = 0;
            instanceCreateInfo.ppEnabledExtensionNames = nullptr; 
        } else {
            instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_RequiredExtensions.size());
            instanceCreateInfo.ppEnabledExtensionNames = m_RequiredExtensions.data();
        }

        if (m_Validater->isValidationEnabled()) {
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_Validater->getLayerSize());
            instanceCreateInfo.ppEnabledLayerNames = m_Validater->getLayerData();
            instanceCreateInfo.pNext = &m_DebugCreateInfo; 
        }
        else {
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.pNext = nullptr;
        }

        auto createInstanceResult = vkCreateInstance(&instanceCreateInfo, pAllocator, &m_Instance);
        switch (createInstanceResult) {
	        case VK_ERROR_LAYER_NOT_PRESENT:
	            Z_LOG_TXT("VulkanAPI::initInstance::vkCreateInstance", "failed to find required layers");
	            Z_LOG_TXT("VulkanAPI::initInstance", "Trying vkCreateInstance again...");

	            instanceCreateInfo.enabledLayerCount = 0;
	            instanceCreateInfo.ppEnabledLayerNames = nullptr;

	            // try to create instance again with no layers
        		createInstanceResult = vkCreateInstance(&instanceCreateInfo, pAllocator, &m_Instance);
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

    VkInstance& Instance::getInstance() {
        return m_Instance;
    }

}
