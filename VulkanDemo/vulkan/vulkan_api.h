#pragma once

#include <vector>
#include <map>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "vk_logger.h"
#include "vk_debug_wrapper.h"
#include "vk_device_wrapper.h"
#include "vk_surface_wrapper.h"
#include "vk_validation_wrapper.h"
#include "vk_swap_chain_wrapper.h"
#include "vk_pipeline_wrapper.h"
#include "vk_cmd_wrapper.h"
#include "vk_sync_wrapper.h"

class VulkanAPI {

	public:

		VulkanAPI(GLFWwindow& _window);
		~VulkanAPI();

		void newFrame();

		void init();

	private:

		_VkLogger& logger; 

		GLFWwindow& window;

		VkInstance vkInstance;

		VkValidationWrapper vkValidationWrapper;
		VkDebugWrapper* vkDebugWrapper;

		VkSurfaceWrapper* vkSurfaceWrapper;

		VkDeviceWrapper* vkDeviceWrapper;
		VkSwapChainWrapper* vkSwapChainWrapper;
		VkPipelineWrapper* vkPipelineWrapper;
		
		VkCmdPoolWrapper* vkCmdPoolWrapper;
		VkCmdBufferWrapper* vkCmdBufferWrapper;

		VkSyncWrapper* vkSyncWrapper;

		void initInstance();
		void initSurface();
		void initDevice();
		void initSwapChain();
		void initPipeline();
		void initCmdWrapper();
		void initSync();

		std::vector<const char*> getRequiredExtensions();

};