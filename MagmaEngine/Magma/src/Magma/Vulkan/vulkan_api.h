#pragma once

#include <vector>
#include <map>

#include <vulkan/vulkan.h>

#include "Magma/Render/render_core.h"
#include "Magma/Window/vulkan_impl.h"

#include "Magma/Vulkan/Command/vulkan_cmd.h"
#include "Magma/Vulkan/Command/vulkan_operation.h"

#include "Magma/Vulkan/Device/device_enums.h"
#include "Magma/Vulkan/Device/device_profile.h"
#include "Magma/Vulkan/Device/vulkan_device.h"

#include "Magma/Vulkan/Image/image_view.h"
#include "Magma/Vulkan/Image/sampler.h"
#include "Magma/Vulkan/Image/vulkan_image.h"

#include "Magma/Vulkan/Logging/vulkan_logger.h"

#include "Magma/Vulkan/Memory/VMA/vma_buffer.h"
#include "Magma/Vulkan/Memory/vulkan_buffer.h"
#include "Magma/Vulkan/Memory/descriptor.h"

#include "Magma/Vulkan/Pipeline/vulkan_pipeline.h"
#include "Magma/Vulkan/Pipeline/vulkan_pipeline_c.h"

#include "Magma/Vulkan/Render/render_enums.h"
#include "Magma/Vulkan/Render/vulkan_framebuffer.h"
#include "Magma/Vulkan/Render/vulkan_renderer.h"
#include "Magma/Vulkan/Render/vulkan_sync.h"

#include "Magma/Vulkan/Setup/vulkan_debugger.h"
#include "Magma/Vulkan/Setup/vulkan_instance.h"
#include "Magma/Vulkan/Setup/vulkan_object.h"
#include "Magma/Vulkan/Setup/vulkan_validater.h"

#include "Magma/Vulkan/Shader/shader_attributes.h"
#include "Magma/Vulkan/Shader/vulkan_shader.h"

#include "Magma/Vulkan/Surface/vulkan_surface.h"
#include "Magma/Vulkan/Surface/vulkan_swapchain.h"

namespace Magma {

	class VulkanAPI : public RenderCore {

	public:
		VulkanAPI(VulkanImpl& windowImpl);
		~VulkanAPI();

		void init() override;
		void initRender() override;
		void onNewFrame() override;

		std::shared_ptr<VulkanDevice> getDevice();

		Renderer& getRenderer() const override;
		ShaderAttributes& getShaderAttributes() const override;

		std::vector<std::shared_ptr<MagmaShader>>& getShaders();

		std::shared_ptr<MagmaShader> createShader(const char* code, ShadercType type);
		std::shared_ptr<MagmaShader> createShader(VulkanShaderInfo info);

		std::shared_ptr<Buffer> createBuffer(int64_t size) override;
		std::shared_ptr<Buffer> createBuffer(int64_t size, BufferUsage bufferUsage) override;

		std::shared_ptr<Image> createTexture(uint32_t width, uint32_t height);
		std::shared_ptr<VulkanImageView> createTextureImageView(std::shared_ptr<Image> image); 
		std::shared_ptr<Sampler> createSampler(); 

		void addBuffer(std::shared_ptr<Buffer> buffer) override; 

		void setDepthBuffering(bool enabled) override;

	private:
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
			VkFormatFeatureFlags features);
		VkFormat findDepthFormat();

		void initCommands();
		void initSync();
		void recreateSwapchain();

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
		std::shared_ptr<VulkanRenderer> vulkanRenderer;
		std::shared_ptr<VulkanPipeline> vulkanPipeline;

		std::shared_ptr<VulkanImage> depthImage; 
		std::shared_ptr<VulkanImageView> depthImageView; 

		VulkanShaderAttributes shaderAttributes{};

		std::vector<std::shared_ptr<VulkanCmdPool>> vulkanCmdPools{};
		std::vector<std::shared_ptr<VulkanCmdBuffer>> vulkanCmdBuffers{};
		std::vector<std::shared_ptr<VulkanRenderSync>> vulkanRenderSyncs{};

		std::vector<std::shared_ptr<MagmaShader>> vulkanShaders{};
		std::vector<std::shared_ptr<VulkanBuffer>> buffers{};

		std::shared_ptr<MagmaShader> defaultVertexShader, defaultFragmentShader;

		std::vector<const char*> getRequiredExtensions();

	};

}