#pragma once

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

		[[nodiscard]] bool shouldWindowClose() const override;
		[[nodiscard]] Int32Size getMonitorResolution() const override;
		[[nodiscard]] Int32Size getSize() const override;
		[[nodiscard]] Int32Size getFramebufferSize() const override;
		[[nodiscard]] std::vector<int32_t> getFrameSize() const override;
		[[nodiscard]] Int32Pos getPosition() const override;
		[[nodiscard]] const std::string& getTitle() const override;
		[[nodiscard]] DoublePos getMousePosition() const override;
		[[nodiscard]] bool isResizable() const override;
		[[nodiscard]] bool isFocused() const override; 

		void setSize(int32_t width, int32_t height) override;
		void setPosition(int32_t x, int32_t y) override;
		void setTitle(std::string title) override;
		void setResizable(bool resizable) override;
		void setMousePosition(double x, double y) override;
		void setMouseHidden(bool hidden) override;
		void setMouseCentered() override;

		MouseAction getMouseAction(int action) override;
		MouseButton getMouseButton(int btn) override;
		KeyAction getKeyAction(int action) override;
		KeyButton getKeyButton(int btn) override;

		VkResult getSurfaceKHR(VkInstance& vkInstance, VkSurfaceKHR& vkSurfaceKHR) override;

	private:
		GLFWwindow* m_Window = nullptr;

		std::string m_Title;

		static GLFWImpl* getUserPtrFromWindow(GLFWwindow* window);

		static void glfwOnWindowResize(GLFWwindow* window, int32_t width, int32_t height);
		static void glfwOnWindowFocus(GLFWwindow* window, int focused); 
		static void glfwOnFramebufferResize(GLFWwindow* window, int32_t width, int32_t height);
		static void glfwOnMouseMove(GLFWwindow* window, double x, double y);
		static void glfwOnMouseScroll(GLFWwindow* window, double x, double y);
		static void glfwOnMousePress(GLFWwindow* window, int button, int action, int mods);
		static void glfwOnMouseEnter(GLFWwindow* window, int entered);
		static void glfwOnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);

	};

}