#pragma once

#include "Vulktrix/Command/vulkan_cmd.h"

#include "Vulktrix/Render/vulkan_drawer.h"
#include "Vulktrix/Render/vulkan_framebuffer.h"

#include "Vulktrix/Shader/shader_attributes.h"

class VulkanPipeline : public VulkanObject {

public:
	VulkanPipeline(ShaderAttributes& pShaderAttributes);
	~VulkanPipeline() override;

	std::shared_ptr<VulkanSwapchain> pVulkanSwapchain = nullptr;
	std::shared_ptr<VulkanDrawer> pVulkanDrawer = nullptr;
	
	ShaderAttributes& pShaderAttributes;

	VkResult init() override;
	VkResult initFramebuffers();

	void addShader(std::shared_ptr<VulkanShader> pVulkanShader);

	void destroyFramebuffers();
	void destroyPipeline();

	void onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer, uint32_t imageIndex);
	
private:
	VulkanShaderPipeline vulkanShaderPipeline{};
	std::shared_ptr<VulkanFixedFunctionState> vulkanFixedFunctionState = nullptr;
	std::shared_ptr<VulkanRenderPass> vulkanRenderPass = nullptr;
	std::shared_ptr<VulkanFramebuffer> vulkanFramebuffer = nullptr;

	VkPipeline vkPipeline{};

};