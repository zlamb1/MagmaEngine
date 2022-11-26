#include "vulkan_surface.h"

VulkanSurface::VulkanSurface(GLFWwindow* pWindow, std::shared_ptr<VulkanInstance> pVulkanInstance) :
    pWindow{ pWindow }, pVulkanInstance{ pVulkanInstance } {}

VulkanSurface::~VulkanSurface() {
    if (pVulkanInstance != nullptr) {
        vkDestroySurfaceKHR(pVulkanInstance->getInstance(), vkSurfaceKHR, nullptr);
    }
}

VkResult VulkanSurface::init() {
    if (pWindow == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanSurface::init", "pWindow is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pVulkanInstance == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanSurface::init", "pVulkanInstance is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    auto createWindowSurface = glfwCreateWindowSurface(
        pVulkanInstance->getInstance(), pWindow, nullptr, &vkSurfaceKHR);
    VulkanLogger::instance().enqueueObject("VulkanSurface::init::glfwCreateWindowSurface", 
        createWindowSurface);

    return createWindowSurface;
}

VkSurfaceKHR& VulkanSurface::getSurfaceKHR() {
    return vkSurfaceKHR;
}