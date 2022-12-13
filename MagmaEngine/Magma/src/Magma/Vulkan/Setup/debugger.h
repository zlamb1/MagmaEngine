#pragma once

#include "vulkan_object.h"

namespace Magma {

	class Debugger : public VulkanObject {

	public:
		Debugger(VkInstance& instance);
		~Debugger() override;

		VkResult init() override;
		static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	private:
		VkInstance& m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger{};

		VkResult createDebugUtilsMessengerExt(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
		void destroyDebugUtilsMessengerExt() const;

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	};

}

