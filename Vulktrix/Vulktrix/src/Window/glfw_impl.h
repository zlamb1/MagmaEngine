#pragma once

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulkan_impl.h"

namespace Window {
	class GLFWImpl : public Window::VulkanImpl {
		public:
			GLFWImpl() = default;
			~GLFWImpl() override;

			void init(int32_t width, int32_t height) override;

			void waitForEvents() override;

			// getters
			bool shouldWindowClose() override;
			Window::Int32Size getMonitorResolution() override;
			Window::Int32Size getSize() override;
			const std::string& getTitle() override;
			bool isResizable() override;
			Window::Int32Size getFramebufferSize() override;

			// setters
			void setResizable(bool resizable) override;
			void setSize(int32_t width, int32_t height) override;
			void setPosition(int32_t x, int32_t y) override;
			void setTitle(std::string title) override;
			void setMousePosition(double x, double y) override;
			void setMouseHidden(bool hidden) override;

			// polymorphic getters
			MouseButton getMouseButton(int btn) override;
			MouseAction getMouseAction(int action) override;

			// Vulkan surface getter
			VkResult getSurfaceKHR(VkInstance& vkInstance,
				VkSurfaceKHR& vkSurfaceKHR) override;

		private:
			// window pointer
			GLFWwindow* window = nullptr;

			// glfw doesn't store title
			std::string title;

			static GLFWImpl* getUserPtrFromWindow(GLFWwindow* window);

			// static callbacks for GLFW
			static void glfwOnWindowResize(GLFWwindow* window, int32_t width, int32_t height);
			static void glfwOnFramebufferResize(GLFWwindow* window, int32_t width, int32_t height);

			static void glfwOnMouseMove(GLFWwindow* window, double x, double y);

			static void glfwOnMouseScroll(GLFWwindow* window, double x, double y);

			static void glfwOnMousePress(GLFWwindow* window, int button, int action, int mods);

			static void glfwOnMouseEnter(GLFWwindow* window, int entered);
	};
}