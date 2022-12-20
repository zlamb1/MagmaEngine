#pragma once

#include <memory>

#include "instance.h"

namespace Magma {

	class Debugger : public VulkanObject {

	public:
		Debugger(std::shared_ptr<Instance> instance);
		~Debugger() override;

		VkResult init() override;
		static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	private:
		std::shared_ptr<Instance> m_Instance; 
		VkDebugUtilsMessengerEXT m_DebugMessenger{};

		VkResult createDebugUtilsMessengerExt(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	};

}

