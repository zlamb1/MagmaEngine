#include "vk_surface_wrapper.h"

_VkSurface::_VkSurface() {

}

_VkSurface::~_VkSurface() {
    if (pInstance != nullptr) {
        vkDestroySurfaceKHR(*pInstance, vkSurfaceKHR, nullptr);
    }
}

VkResult _VkSurface::create() {
    auto _vkLogger = _VkLogger::Instance();

    if (pWindow == nullptr) {
        _vkLogger.LogText("_VkSurface => pWindow is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pInstance == nullptr) {
        _vkLogger.LogText("_VkSurface => pInstance is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED; 
    }

    auto vkCreateWindowSurface = glfwCreateWindowSurface(
        *pInstance, pWindow, nullptr, &vkSurfaceKHR);
    _vkLogger.LogResult("glfwCreateWindowSurface =>", vkCreateWindowSurface);

    return vkCreateWindowSurface;
}