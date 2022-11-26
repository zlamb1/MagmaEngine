#pragma once

#include <vector>
#include <map>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "vulkan_logger.h"
#include "vulkan_debugger.h"
#include "vulkan_device.h"
#include "vulkan_surface.h"
#include "vulkan_validater.h"
#include "vulkan_swapchain.h"
#include "vulkan_pipeline.h"
#include "vulkan_cmd.h"
#include "vulkan_sync.h"
#include "vulkan_buffer.h"
#include "vulkan_instance.h"

#include "vulkan_vertex_state.h"

class VulkanAPI {

public:
	VulkanAPI() = default;
	~VulkanAPI();

	void initSetup(GLFWwindow* glfwWindow);
	void initRender();
	void onNewFrame(uint32_t vertexCount);

	std::shared_ptr<VulkanDevice> getVulkanDevice();

	std::vector<std::shared_ptr<VulkanShader>>& getShaderHandles();
	std::vector<std::shared_ptr<VulkanBuffer>>& getBufferHandles();
	
	void addVertexInputState(VulkanVertexState& vertexState);
	void setFramebufferResized(bool framebufferResized);
	
	std::shared_ptr<VulkanShader> createShaderHandle(const char* code, VulkanShaderType type);
	std::shared_ptr<VulkanShader> createShaderHandle(VulkanShaderInfo info);

	std::shared_ptr<VulkanBuffer> createBufferHandle(VkDeviceSize pSize);
	std::shared_ptr<VulkanBuffer> createBufferHandle(VkDeviceSize pSize, VulkanBufferUsage pBufferUsage,
		VulkanMemoryType pMemType);

private:
	const int MAX_FRAMES_IN_FLIGHT = 1;

	uint32_t currentFrame = 0;

	bool framebufferResized = false;

	GLFWwindow* glfwWindow = nullptr;

	std::shared_ptr<VulkanInstance> vulkanInstance;
	std::shared_ptr<VulkanValidater> vulkanValidater;
	std::shared_ptr<VulkanDebugger> vulkanDebugger;
	std::shared_ptr<VulkanSurface> vulkanSurface;
	std::shared_ptr<VulkanDevice> vulkanDevice;
	std::shared_ptr<VulkanSwapchain> vulkanSwapchain;
	std::shared_ptr<VulkanDrawer> vulkanDrawer; 
	std::shared_ptr<VulkanPipeline> vulkanPipeline;
		
	std::vector<std::shared_ptr<VulkanCmdPool>> vulkanCmdPools{};
	std::vector<std::shared_ptr<VulkanCmdBuffer>> vulkanCmdBuffers{};
	std::vector<std::shared_ptr<VulkanRenderSync>> vulkanRenderSyncs{};
	
	std::vector<std::shared_ptr<VulkanShader>> vulkanShaders{};
	std::vector<std::shared_ptr<VulkanBuffer>> vulkanBuffers{};

	std::shared_ptr<VulkanShader> defaultVertexShader, defaultFragmentShader;

	void initCommands();
	void initSync();

	void recreateSwapchain();

	std::vector<const char*> getRequiredExtensions();

};