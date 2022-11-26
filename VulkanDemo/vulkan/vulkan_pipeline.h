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
	VulkanPipeline() = default;
	~VulkanPipeline() override;

	VulkanDevice* pDevice = nullptr;
	VulkanSwapchain* pSwapchain = nullptr;
	VulkanDrawer* pVulkanDrawer = nullptr;

	std::vector<VkVertexInputBindingDescription> pBindingDescriptions{};
	std::vector<VkVertexInputAttributeDescription> pAttributeDescriptions{};

	VkResult init() override;
	VkResult initFramebuffers();

	void addShader(VulkanShader* _vkShaderHandle);
	void deleteFramebuffers();

	void onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer, uint32_t imageIndex);

private:
	VulkanShaderPipeline vulkanShaderPipeline;
	std::unique_ptr<VulkanFixedFunctionState> vulkanFixedFuctionState;
	std::unique_ptr<VulkanRenderPass> vulkanRenderPass;
	VulkanFramebuffer* vulkanFramebuffer = nullptr;

	VkPipeline vkPipeline{};

};