#include "vulkan_surface.h"

VulkanSurface::VulkanSurface(Window::VulkanImpl& pWindowImpl,
    std::shared_ptr<VulkanInstance> pVulkanInstance) :
    pWindowImpl{ pWindowImpl }, pVulkanInstance{ pVulkanInstance } {}

VulkanSurface::~VulkanSurface() {
    if (pVulkanInstance != nullptr) {
        vkDestroySurfaceKHR(pVulkanInstance->getInstance(), vkSurfaceKHR, nullptr);
    }
}

VkResult VulkanSurface::init() {
    if (pVulkanInstance == nullptr) {
        Z_LOG_TXT("VulkanSurface::init", "pVulkanInstance is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    auto createWindowSurfaceResult = pWindowImpl.getSurfaceKHR(
        pVulkanInstance->getInstance(), vkSurfaceKHR);
    Z_LOG_OBJ("VulkanSurface::init::glfwCreateWindowSurface", createWindowSurfaceResult);

    return createWindowSurfaceResult;
}

VkSurfaceKHR& VulkanSurface::getSurfaceKHR() {
    return vkSurfaceKHR;
}