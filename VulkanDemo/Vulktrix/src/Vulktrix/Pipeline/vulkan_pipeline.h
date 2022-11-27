#pragma once

#include "Vulktrix/Command/vulkan_cmd.h"

#include "Vulktrix/Memory/vulkan_vertex_state.h"

#include "Vulktrix/Render/vulkan_drawer.h"
#include "Vulktrix/Render/vulkan_framebuffer.h"

class VulkanPipeline : public VulkanObject {

public:
	VulkanPipeline(std::shared_ptr<VulkanSwapchain> pVulkanSwapchain);
	~VulkanPipeline() override;

	std::shared_ptr<VulkanSwapchain> pVulkanSwapchain = nullptr;
	std::shared_ptr<VulkanDrawer> pVulkanDrawer = nullptr;
	
	std::vector<VkVertexInputBindingDescription> pBindingDescriptions{};
	std::vector<VkVertexInputAttributeDescription> pAttributeDescriptions{};

	std::shared_ptr<VulkanDescriptorSetLayout> pVulkanDescriptorSetLayout = nullptr;

	VkResult init() override;
	VkResult initFramebuffers();

	void addShader(std::shared_ptr<VulkanShader> pVulkanShader);
	void destroyFramebuffers();

	void onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer, uint32_t imageIndex);

private:
	VulkanShaderPipeline vulkanShaderPipeline{};
	std::shared_ptr<VulkanFixedFunctionState> vulkanFixedFunctionState = nullptr;
	std::shared_ptr<VulkanRenderPass> vulkanRenderPass = nullptr;
	std::shared_ptr<VulkanFramebuffer> vulkanFramebuffer = nullptr;

	VkPipeline vkPipeline{};

};