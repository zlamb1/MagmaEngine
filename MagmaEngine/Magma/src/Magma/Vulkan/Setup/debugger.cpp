#include "debugger.h"

#include "Magma/Vulkan/Logging/vulkan_logger.h"

namespace Magma {

    Debugger::Debugger(std::shared_ptr<Instance> instance) : m_Instance{ std::move(instance) } {

    }

    Debugger::~Debugger() {
        PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT = VK_NULL_HANDLE;
        DestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
            m_Instance->getInstance(), "vkDestroyDebugUtilsMessengerEXT"));
        if (DestroyDebugUtilsMessengerEXT != nullptr) {
            DestroyDebugUtilsMessengerEXT(m_Instance->getInstance(), m_DebugMessenger, pAllocator); 
        }
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
        const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
            m_Instance->getInstance(),"vkCreateDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            return func(m_Instance->getInstance(), pCreateInfo, pAllocator, &m_DebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
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