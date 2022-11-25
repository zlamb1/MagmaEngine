#include "vulkan_surface.h"

VulkanSurface::~VulkanSurface() {
    if (pInstance != nullptr) {
        vkDestroySurfaceKHR(*pInstance, vkSurfaceKHR, nullptr);
    }
}

VkResult VulkanSurface::init() {
    if (pWindow == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanSurface::init", "pWindow is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pInstance == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanSurface::init", "pInstance is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    auto createWindowSurface = glfwCreateWindowSurface(
        *pInstance, pWindow, nullptr, &vkSurfaceKHR);
    VulkanLogger::instance().enqueueObject("VulkanSurface::init::glfwCreateWindowSurface", 
        createWindowSurface);

    return createWindowSurface;
}

VkSurfaceKHR& VulkanSurface::getSurfaceKHR() {
    return vkSurfaceKHR;
}