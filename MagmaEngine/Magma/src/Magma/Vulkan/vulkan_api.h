#pragma once

#include <vector>
#include <map>

#include <vulkan/vulkan.h>

#include "Window/vulkan_impl.h"

// all Magma includes

#include "Vulkan/Command/vulkan_cmd.h"
#include "Vulkan/Command/vulkan_operation.h"

#include "Vulkan/Device/device_enums.h"
#include "Vulkan/Device/device_profile.h"
#include "Vulkan/Device/vulkan_device.h"

#include "Vulkan/Logging/vulkan_logger.h"

#include "Vulkan/Memory/vulkan_buffer.h"
#include "Vulkan/Memory/vulkan_buffer_copy.h"
#include "Vulkan/Memory/descriptor.h"

#include "Vulkan/Pipeline/vulkan_pipeline.h"
#include "Vulkan/Pipeline/vulkan_pipeline_c.h"

#include "Vulkan/Render/vulkan_drawer.h"
#include "Vulkan/Render/vulkan_framebuffer.h"
#include "Vulkan/Render/vulkan_sync.h"

#include "Vulkan/Setup/vulkan_debugger.h"
#include "Vulkan/Setup/vulkan_instance.h"
#include "Vulkan/Setup/vulkan_object.h"
#include "Vulkan/Setup/vulkan_validater.h"

#include "Vulkan/Shader/vulkan_shader.h"
#include "Vulkan/Shader/shader_attributes.h"

#include "Vulkan/Surface/vulkan_surface.h"
#include "Vulkan/Surface/vulkan_swapchain.h"

namespace Magma {

	class VulkanAPI {

	public:
		VulkanAPI(VulkanImpl& windowImpl);
		~VulkanAPI();

		void initSetup();
		void initRender();
		void onNewFrame(uint32_t vertexCount);

		std::shared_ptr<VulkanDevice> getVulkanDevice();
		std::shared_ptr<VulkanDrawer> getVulkanDrawer();

		std::vector<std::shared_ptr<MagmaShader>>& getVulkanShaders();
		std::vector<std::shared_ptr<VulkanBuffer>>& getVulkanBuffers();

		void setFramebufferResized(bool framebufferResized);

		std::shared_ptr<MagmaShader> createVulkanShader(const char* code, ShadercType type);
		std::shared_ptr<MagmaShader> createVulkanShader(VulkanShaderInfo info);

		std::shared_ptr<VulkanBuffer> createVulkanBuffer(VkDeviceSize pSize);
		std::shared_ptr<VulkanBuffer> createVulkanBuffer(VkDeviceSize pSize, VulkanBufferUsage pBufferUsage);

		ShaderAttributes& getShaderAttributes();

	private:
		const int MAX_FRAMES_IN_FLIGHT = 1;

		uint32_t currentFrame = 0;

		bool framebufferResized = false;

		VulkanImpl& windowImpl;

		std::shared_ptr<VulkanInstance> vulkanInstance;
		std::shared_ptr<VulkanValidater> vulkanValidater;
		std::shared_ptr<VulkanDebugger> vulkanDebugger;
		std::shared_ptr<VulkanSurface> vulkanSurface;
		std::shared_ptr<VulkanDevice> vulkanDevice;
		std::shared_ptr<VulkanSwapchain> vulkanSwapchain;
		std::shared_ptr<VulkanDrawer> vulkanDrawer;
		std::shared_ptr<VulkanPipeline> vulkanPipeline;

		ShaderAttributes shaderAttributes{};

		std::vector<std::shared_ptr<VulkanCmdPool>> vulkanCmdPools{};
		std::vector<std::shared_ptr<VulkanCmdBuffer>> vulkanCmdBuffers{};
		std::vector<std::shared_ptr<VulkanRenderSync>> vulkanRenderSyncs{};

		std::vector<std::shared_ptr<MagmaShader>> vulkanShaders{};
		std::vector<std::shared_ptr<VulkanBuffer>> vulkanBuffers{};

		std::shared_ptr<MagmaShader> defaultVertexShader, defaultFragmentShader;

		void initCommands();
		void initSync();

		void recreateSwapchain();

		std::vector<const char*> getRequiredExtensions();

	};

}