#include "vk_surface_wrapper.h"

// Public

VkSurfaceWrapper::VkSurfaceWrapper(GLFWwindow& _glfwWindow, VkInstance& _vkInstance) :
    glfwWindow{ _glfwWindow }, vkInstance{ _vkInstance }  
{

}

VkSurfaceWrapper::~VkSurfaceWrapper() {
    vkDestroySurfaceKHR(vkInstance, vkSurfaceKHR, nullptr);
}

VkSurfaceKHR& VkSurfaceWrapper::GetSurfaceKHR() {
    return vkSurfaceKHR;
}

void VkSurfaceWrapper::Initialize() {
    auto logger = _VkLogger::Instance();
    auto createWindowSurfaceResult = glfwCreateWindowSurface(
        vkInstance, &glfwWindow, nullptr, &vkSurfaceKHR);
    if (createWindowSurfaceResult != VK_SUCCESS) {
        logger.LogResult("glfwCreateWindowSurface =>", createWindowSurfaceResult);
    }
}