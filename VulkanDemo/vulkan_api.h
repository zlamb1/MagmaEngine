#pragma once

#include <vector>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "vk_debug_wrapper.h"
#include "vk_device_wrapper.h"
#include "vk_surface_wrapper.h"
#include "vk_validation_wrapper.h"
#include "vk_swap_chain_wrapper.h"
#include "vk_pipeline_wrapper.h"
#include "vk_command_wrapper.h"
#include "vk_sync_wrapper.h"

class VulkanAPI {

	public:

		VulkanAPI(GLFWwindow& _window);
		~VulkanAPI();

		void newFrame();

		void init();

	private:

		GLFWwindow& window;

		VkInstance vkInstance;

		VkValidationWrapper vkValidationWrapper;

		std::unique_ptr<VkDebugWrapper> vkDebugWrapper;
		std::unique_ptr<VkSurfaceWrapper> vkSurfaceWrapper;
		std::unique_ptr<VkDeviceWrapper> vkDeviceWrapper;
		std::unique_ptr<VkSwapChainWrapper> vkSwapChainWrapper;
		std::unique_ptr<VkPipelineWrapper> vkPipelineWrapper;
		std::unique_ptr<VkCommandWrapper> vkCommandWrapper;
		std::unique_ptr<VkSyncWrapper> vkSyncWrapper;

		void initInstance();
		void initSurface();
		void initDevice();
		void initSwapChain();
		void initPipeline();
		void initCommands();
		void initSync();

		std::vector<const char*> getRequiredExtensions();

};