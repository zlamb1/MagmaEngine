#include "surface.h"

namespace Magma {

    Surface::Surface(VulkanImpl& windowImpl,
        std::shared_ptr<Instance> instance) :
        m_WindowImpl{ windowImpl }, m_Instance{ std::move(instance) } {}

    Surface::~Surface() {
        if (m_Instance != nullptr) {
            vkDestroySurfaceKHR(m_Instance->getInstance(), m_SurfaceKHR, nullptr);
        }
    }

    VkResult Surface::init() {
        if (m_Instance == nullptr) {
            Z_LOG_TXT("VulkanSurface::init", "pVulkanInstance is nullptr");
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        auto createWindowSurfaceResult = m_WindowImpl.getSurfaceKHR(
            m_Instance->getInstance(), m_SurfaceKHR);
        Z_LOG_OBJ("VulkanSurface::init::glfwCreateWindowSurface", createWindowSurfaceResult);

        return createWindowSurfaceResult;
    }

    VkSurfaceKHR& Surface::getSurfaceKHR() {
        return m_SurfaceKHR;
    }

}