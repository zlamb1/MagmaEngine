#pragma once

#include <iostream>

#include <vulkan/vulkan.h>

#include "vulkan_object.h"

class VulkanDebugger : public VulkanObject {

public:

	VulkanDebugger(VkInstance& _instance);
	~VulkanDebugger() override;

	VkResult init() override;
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

private:

	VkInstance& instance;
	VkDebugUtilsMessengerEXT debugMessenger{};

	VkResult CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator);
	void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator);

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

};