#pragma once

#include <vector>

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

	class VulkanCore : public RenderCore {

	public:
		VulkanCore(VulkanImpl& windowImpl);
		virtual ~VulkanCore();

		void init() override;
		void initRender() override;
		void onNewFrame() override;

		std::shared_ptr<VulkanDevice> getDevice();

		[[nodiscard]] Renderer& getRenderer() const override;
		[[nodiscard]] ShaderAttributes& getShaderAttributes() override;

		std::vector<std::shared_ptr<MagmaShader>>& getShaders();

		std::shared_ptr<MagmaShader> createShader(const char* code, ShadercType type);
		std::shared_ptr<MagmaShader> createShader(VulkanShaderInfo shaderInfo);

		std::shared_ptr<Buffer> createBuffer(int64_t size) override;
		std::shared_ptr<Buffer> createBuffer(int64_t size, BufferUsage bufferUsage) override;

		std::shared_ptr<Image> createTexture(uint32_t width, uint32_t height);
		std::shared_ptr<VulkanImageView> createTextureImageView(std::shared_ptr<Image> image); 
		[[nodiscard]] std::shared_ptr<Sampler> createSampler() const; 

		void addBuffer(std::shared_ptr<Buffer> buffer) override; 

		void setDepthBuffering(bool enabled) override;

	private:
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
			VkFormatFeatureFlags features) const;
		VkFormat findDepthFormat() const;

		void initCommands();
		void initSync();
		void recreateSwapchain();

	private:
		const int MAX_FRAMES_IN_FLIGHT = 1;

		uint32_t m_CurrentFrame = 0;

		bool m_FramebufferResized = false;

		VulkanImpl& m_WindowImpl;

		std::shared_ptr<VulkanInstance> m_Instance;
		std::shared_ptr<VulkanValidater> m_Validater;
		std::shared_ptr<VulkanDebugger> m_Debugger;
		std::shared_ptr<VulkanSurface> m_Surface;
		std::shared_ptr<VulkanDevice> m_Device;
		std::shared_ptr<VulkanSwapchain> m_Swapchain;
		std::shared_ptr<VulkanRenderer> m_Renderer;
		std::shared_ptr<VulkanPipeline> m_Pipeline;

		std::shared_ptr<VulkanImage> m_DepthImage; 
		std::shared_ptr<VulkanImageView> m_DepthImageView; 

		VulkanShaderAttributes m_ShaderAttributes{};

		std::vector<std::shared_ptr<VulkanCmdPool>> m_CommandPools{};
		std::vector<std::shared_ptr<VulkanCmdBuffer>> m_CommandBuffers{};
		std::vector<std::shared_ptr<VulkanRenderSync>> m_RenderSyncs{};

		std::vector<std::shared_ptr<MagmaShader>> m_Shaders{};
		std::vector<std::shared_ptr<VulkanBuffer>> m_Buffers{};

		std::shared_ptr<MagmaShader> m_VertexShader, m_FragmentShader;

		std::vector<const char*> getRequiredExtensions() const;

	};

}