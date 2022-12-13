#include "debugger.h"

#include "Magma/Vulkan/Logging/vulkan_logger.h"

namespace Magma {

    Debugger::Debugger(VkInstance& instance) : m_Instance{ instance } {

    }

    Debugger::~Debugger() {
        destroyDebugUtilsMessengerExt();
    }

    VkResult Debugger::init() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);
        auto createDebugUtilsMessengerResult = createDebugUtilsMessengerExt(&createInfo);
            Z_LOG_OBJ("VulkanDebugger::init", createDebugUtilsMessengerResult);
        return createDebugUtilsMessengerResult;
    }

    void Debugger::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    VkResult Debugger::createDebugUtilsMessengerExt(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo) {
        const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, 
		"vkCreateDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            return func(m_Instance, pCreateInfo, pAllocator, &m_DebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void Debugger::destroyDebugUtilsMessengerExt() const {
        const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance,
            "vkDestroyDebugUtilsMesssengerEXT"));
        if (func != nullptr) {
            func(m_Instance, m_DebugMessenger, pAllocator);
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Debugger::debugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
            Z_LOG_TXT("VulkanDebugger::ValidationLayer", pCallbackData->pMessage);
        }
        return VK_FALSE;
    }

}