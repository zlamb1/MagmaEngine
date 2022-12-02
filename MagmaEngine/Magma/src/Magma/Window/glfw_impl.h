#pragma once

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulkan_impl.h"

namespace Magma {

	class GLFWImpl : public VulkanImpl {

	public:
		GLFWImpl() = default;
		~GLFWImpl() override;

		void init(int32_t width, int32_t height) override;

		void waitForEvents() override;

		bool shouldWindowClose() override;
		Int32Size getMonitorResolution() override;
		Int32Size getSize() override;
		Int32Pos getPosition() override;
		const std::string& getTitle() override;
		bool isResizable() override;
		Int32Size getFramebufferSize() override;

		void setResizable(bool resizable) override;
		void setSize(int32_t width, int32_t height) override;
		void setPosition(int32_t x, int32_t y) override;
		void setTitle(std::string title) override;
		void setMousePosition(double x, double y) override;
		void setMouseHidden(bool hidden) override;

		MouseButton getMouseButton(int btn) override;
		MouseAction getMouseAction(int action) override;

		VkResult getSurfaceKHR(VkInstance& vkInstance, VkSurfaceKHR& vkSurfaceKHR) override;

	private:
		GLFWwindow* window = nullptr;

		std::string title;

		static GLFWImpl* getUserPtrFromWindow(GLFWwindow* window);

		static void glfwOnWindowResize(GLFWwindow* window, int32_t width, int32_t height);
		static void glfwOnFramebufferResize(GLFWwindow* window, int32_t width, int32_t height);
		static void glfwOnMouseMove(GLFWwindow* window, double x, double y);
		static void glfwOnMouseScroll(GLFWwindow* window, double x, double y);
		static void glfwOnMousePress(GLFWwindow* window, int button, int action, int mods);
		static void glfwOnMouseEnter(GLFWwindow* window, int entered);

	};

}