#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_device.h"
#include "vulkan_pipeline_c.h"
#include "vulkan_framebuffer.h"
#include "vulkan_cmd.h"
#include "vulkan_vertex_state.h"
#include "vulkan_buffer.h"
#include "vulkan_swapchain.h"
#include "vulkan_drawer.h"

class VulkanPipeline : public VulkanObject {

public:
	VulkanPipeline(std::shared_ptr<VulkanSwapchain> pVulkanSwapchain);
	~VulkanPipeline() override;

	std::shared_ptr<VulkanSwapchain> pVulkanSwapchain = nullptr;
	std::shared_ptr<VulkanDrawer> pVulkanDrawer = nullptr;

	std::vector<VkVertexInputBindingDescription> pBindingDescriptions{};
	std::vector<VkVertexInputAttributeDescription> pAttributeDescriptions{};

	VkResult init() override;
	VkResult initFramebuffers();

	void addShader(std::shared_ptr<VulkanShader> pVulkanShader);
	void destroyFramebuffers();

	void onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer, uint32_t imageIndex);

private:
	VulkanShaderPipeline vulkanShaderPipeline{};
	std::shared_ptr<VulkanFixedFunctionState> vulkanFixedFuctionState = nullptr;
	std::shared_ptr<VulkanRenderPass> vulkanRenderPass = nullptr;
	std::shared_ptr<VulkanFramebuffer> vulkanFramebuffer = nullptr;

	VkPipeline vkPipeline{};

};