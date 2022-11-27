#pragma once

#include <vector>
#include <map>

#include <vulkan/vulkan.h>

#include "Window/vulkan_window.h"

// all Vulktrix includes

#include "Vulktrix/Command/vulkan_cmd.h"
#include "Vulktrix/Command/vulkan_operation.h"

#include "Vulktrix/Device/device_enums.h"
#include "Vulktrix/Device/device_profile.h"
#include "Vulktrix/Device/vulkan_device.h"

#include "Vulktrix/Logging/vulkan_debugger.h"
#include "Vulktrix/Logging/vulkan_logger.h"
#include "Vulktrix/Logging/vulkan_validater.h"

#include "Vulktrix/Memory/vulkan_buffer.h"
#include "Vulktrix/Memory/vulkan_buffer_copy.h"
#include "Vulktrix/Memory/vulkan_descriptor.h"
#include "Vulktrix/Memory/vulkan_vertex_state.h"

#include "Vulktrix/Pipeline/vulkan_pipeline.h"
#include "Vulktrix/Pipeline/vulkan_pipeline_c.h"
#include "Vulktrix/Pipeline/vulkan_shader.h"

#include "Vulktrix/Render/vulkan_drawer.h"
#include "Vulktrix/Render/vulkan_framebuffer.h"
#include "Vulktrix/Render/vulkan_sync.h"

#include "Vulktrix/Setup/vulkan_instance.h"
#include "Vulktrix/Setup/vulkan_object.h"

#include "Vulktrix/Surface/vulkan_surface.h"
#include "Vulktrix/Surface/vulkan_swapchain.h"

class VulktrixAPI {

public:
	VulktrixAPI(VulkanWindow& windowImpl);
	~VulktrixAPI();

	void initSetup();
	void initRender();
	void onNewFrame(uint32_t vertexCount);

	std::shared_ptr<VulkanDevice> getVulkanDevice();
	std::shared_ptr<VulkanDrawer> getVulkanDrawer(); 

	std::vector<std::shared_ptr<VulkanShader>>& getVulkanShaders();
	std::vector<std::shared_ptr<VulkanBuffer>>& getVulkanBuffers();
	
	void addVertexInputState(VulkanVertexState& vertexState);

	void setFramebufferResized(bool framebufferResized);
	
	std::shared_ptr<VulkanShader> createVulkanShader(const char* code, ShadercType type);
	std::shared_ptr<VulkanShader> createVulkanShader(VulkanShaderInfo info);

	std::shared_ptr<VulkanBuffer> createVulkanBuffer(VkDeviceSize pSize);
	std::shared_ptr<VulkanBuffer> createVulkanBuffer(VkDeviceSize pSize, VulkanBufferUsage pBufferUsage);

	std::shared_ptr<VulkanDeviceMemory> createDeviceMemory(
		std::shared_ptr<VulkanBuffer> expectedBufferSpec, VulkanMemoryType pMemType);

	std::shared_ptr<VulkanDescriptorSetLayout> createDescriptorSetLayout(
		std::vector<VulkanDescriptor> vulkanDescriptors);

	void setDescriptorSetLayout(
		std::shared_ptr<VulkanDescriptorSetLayout> vulkanDescriptorSetLayout);

private:
	const int MAX_FRAMES_IN_FLIGHT = 1;

	uint32_t currentFrame = 0;

	bool framebufferResized = false;

	VulkanWindow& windowImpl;

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