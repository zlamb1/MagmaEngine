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
#include "vulkan_deque.h"
#include "vulkan_buffer.h"

#include "vulkan_vertex_state.h"

class VulkanAPI {

public:
	VulkanAPI() = default;
	~VulkanAPI();

	void initSetup(GLFWwindow* glfwWindow);
	void initRender();
	void onNewFrame(uint32_t vertexCount);

	VulkanDevice* getVulkanDevice();

	std::vector<VulkanShader*>& getShaderHandles();
	std::vector<VulkanBuffer*>& getBufferHandles();
	
	void addVertexInputState(VulkanVertexState& vertexState);
	void setFramebufferResized(bool framebufferResized);
	
	VulkanShader* createShaderHandle(const char* code, VulkanShaderType type);
	VulkanShader* createShaderHandle(VulkanShaderInfo info);

	VulkanBuffer* createBufferHandle(uint32_t pSize);
	VulkanBuffer* createBufferHandle(VkDeviceSize pSize, VulkanBufferUsage pBufferUsage,
		VulkanMemoryType pMemType);

private:
	const int MAX_FRAMES_IN_FLIGHT = 1;

	uint32_t currentFrame = 0;

	bool framebufferResized = false;

	GLFWwindow* glfwWindow = nullptr;
	VkInstance vulkanInstance{};

	VulkanDeque vulkanDeque{};
	VulkanValidater vulkanValidater{};
	VulkanDebugger* vulkanDebugger = nullptr;
	VulkanSurface* vulkanSurface = nullptr;
	VulkanDevice* vulkanDevice = nullptr;
	VulkanSwapchain* vulkanSwapchain = nullptr;
	VulkanPipeline* vulkanPipeline = nullptr;
		
	std::vector<VulkanCmdPool*> vulkanCmdPools{};
	std::vector<VulkanCmdBuffer*> vulkanCmdBuffers{};
	std::vector<VulkanRenderSync*> vulkanRenderSyncs{};
	
	std::vector<VulkanShader*> vulkanShaderHandles{};
	std::vector<VulkanBuffer*> vulkanBufferHandles{};

	VulkanShader *defaultVertexShader = nullptr, *defaultFragmentShader = nullptr;

	void initInstance();

	void initCommands();
	void initSync();

	void recreateSwapchain();

	std::vector<const char*> getRequiredExtensions();

};