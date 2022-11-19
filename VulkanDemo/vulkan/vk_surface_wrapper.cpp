#include "vk_surface_wrapper.h"

// Public

VkSurfaceWrapper::VkSurfaceWrapper(VkInstance& _vkInstance, GLFWwindow& _window) :
    vkInstance{ _vkInstance }, window{ _window } {
    initSurface();
}

VkSurfaceWrapper::~VkSurfaceWrapper() {
    vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
}

VkSurfaceKHR& VkSurfaceWrapper::getSurface() {
    return vkSurface;
}

// Private

void VkSurfaceWrapper::initSurface() {
    if (glfwCreateWindowSurface(vkInstance, &window, nullptr, &vkSurface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    else {
        std::cout << "Vulkan surface created." << std::endl << std::endl;
    }
}