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
        Z_LOG_TXT("VulkanSurface::init", "pWindow is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pVulkanInstance == nullptr) {
        Z_LOG_TXT("VulkanSurface::init", "pVulkanInstance is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    auto createWindowSurfaceResult = glfwCreateWindowSurface(
        pVulkanInstance->getInstance(), pWindow, nullptr, &vkSurfaceKHR);
    Z_LOG_OBJ("VulkanSurface::init::glfwCreateWindowSurface", createWindowSurfaceResult);

    return createWindowSurfaceResult;
}

VkSurfaceKHR& VulkanSurface::getSurfaceKHR() {
    return vkSurfaceKHR;
}