#pragma once

#include <iostream>

#include <vulkan/vulkan.h>



class VkDebugWrapper {

public:

	VkDebugWrapper(VkInstance& _instance);
	~VkDebugWrapper();

	void init();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

private:

	VkInstance& instance;
	VkDebugUtilsMessengerEXT debugMessenger;

	VkResult CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator);
	void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

};