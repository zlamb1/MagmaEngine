#include "vulkan_api.h"

const char* defVertexShader = R"(#version 450
    void main() {
        gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    })";

const char* defFragmentShader = R"(#version 450
    layout(location = 0) out vec3 fragColor;
    void main() {
        fragColor = vec3(1, 1, 1);
    })";

namespace Magma {

    VulkanAPI::VulkanAPI(VulkanImpl& windowImpl) : windowImpl{ windowImpl } {
        // add framebuffer resized callback
        windowImpl.addFramebufferSizeCallback([&](int32_t w, int32_t h) {
            framebufferResized = true; 
        });
    }

    VulkanAPI::~VulkanAPI() {
        // allows for clean exit 
        vkDeviceWaitIdle(vulkanDevice->getDevice());
    }

    void VulkanAPI::init() {
        vulkanValidater = std::make_shared<VulkanValidater>();
        vulkanInstance = std::make_shared<VulkanInstance>(vulkanValidater);

        if (vulkanValidater->isValidationEnabled()) {
            vulkanDebugger = std::make_shared<VulkanDebugger>(vulkanInstance->getInstance());
            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            vulkanDebugger->PopulateDebugMessengerCreateInfo(debugCreateInfo);
            vulkanInstance->pDebugCreateInfo = debugCreateInfo;
        }

        if (vulkanValidater->isValidationEnabled() && !vulkanValidater->ensureRequiredLayers()) {
            // TODO: add check to see if layers are strictly necessary
            Z_LOG_TXT("VulkanAPI::initInstance", "Validation layers were requested but are not available");
        }

        vulkanInstance->pRequiredExtensions = getRequiredExtensions();
        vulkanInstance->init();

        if (vulkanValidater->isValidationEnabled()) {
            vulkanDebugger->init();
        }

        // create fields
        vulkanSurface = std::make_shared<VulkanSurface>(windowImpl, vulkanInstance);
        vulkanSurface->init();

        vulkanDevice = std::make_shared<VulkanDevice>(vulkanInstance, vulkanSurface, vulkanValidater);
        vulkanDevice->init();

        shaderAttributes.pVulkanDevice = vulkanDevice;

        vulkanSwapchain = std::make_shared<VulkanSwapchain>(vulkanDevice);
        vulkanSwapchain->init();

        vulkanRenderer = std::make_shared<VulkanRenderer>();

        vulkanPipeline = std::make_shared<VulkanPipeline>(shaderAttributes);

        auto& swapchainExtent = vulkanSwapchain->getSwapchainExtent();

        depthImage = std::make_shared<VulkanImage>(vulkanDevice);
        depthImage->pExtentWidth = swapchainExtent.width;
        depthImage->pExtentHeight = swapchainExtent.height;

        const auto depthFormat = findDepthFormat();
        depthImage->pFormat = findDepthFormat();

        depthImage->pImageTiling = VK_IMAGE_TILING_OPTIMAL;
        depthImage->pUsageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthImage->init();

        depthImageView = std::make_shared<VulkanImageView>(vulkanDevice, depthImage);
        depthImageView->pImageAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
        depthImageView->init();

        vulkanPipeline->pDepthImageView = depthImageView;

        defaultVertexShader = createShader(defVertexShader, ShadercType::VERTEX);
        defaultFragmentShader = createShader(defFragmentShader, ShadercType::FRAGMENT);
    }

    void VulkanAPI::initRender() {
        vulkanRenderer->pDescriptorSets = shaderAttributes.getDescriptorSets();

        vulkanPipeline->pVulkanSwapchain = vulkanSwapchain;
        vulkanPipeline->pVulkanRenderer = vulkanRenderer;

        if (vulkanShaders.empty()) {
            Z_LOG_TXT("VulkanAPI::initRender", "warning: no shaders provided, using defaults");
            vulkanPipeline->addShader(defaultVertexShader);
            vulkanPipeline->addShader(defaultFragmentShader);
        }

        for (auto vulkanShader : vulkanShaders) {
            vulkanPipeline->addShader(vulkanShader);
        }

        for (auto buffer : buffers) {
            vulkanRenderer->pVertexBuffers.push_back(buffer);
        }

        vulkanPipeline->init();

        initCommands();
        initSync();
    }

    void VulkanAPI::onNewFrame() {
        vkWaitForFences(vulkanDevice->getDevice(), 1,
            &vulkanRenderSyncs[currentFrame]->getFlightFence().getFence(), VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        auto vkAcquireNextImageResult = vkAcquireNextImageKHR(vulkanDevice->getDevice(),
            vulkanSwapchain->getSwapchainKHR(), UINT64_MAX,
            vulkanRenderSyncs[currentFrame]->getImageSemaphore().getSemaphore(), VK_NULL_HANDLE, &imageIndex);

        if (vkAcquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapchain();
            return;
        }
        else if (vkAcquireNextImageResult != VK_SUCCESS) {
            Z_LOG_TXT("VulkanAPI::onNewFrame", "failed to acquire a new swapchain image!");
            return;
        }

        vkResetFences(vulkanDevice->getDevice(), 1,
            &vulkanRenderSyncs[currentFrame]->getFlightFence().getFence());

        vulkanPipeline->onNewFrame(*vulkanCmdBuffers[currentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { vulkanRenderSyncs[currentFrame]->getImageSemaphore().getSemaphore() };
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vulkanCmdBuffers[currentFrame]->getCmdBuffer();

        VkSemaphore signalSemaphores[] = {
            vulkanRenderSyncs[currentFrame]->getRenderSemaphore().getSemaphore()
        };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        auto graphicsQueueOpt = DeviceProfile::getQueue(VulkanQueueType::GRAPHICS);
        if (!graphicsQueueOpt.has_value()) {
            Z_LOG_TXT("VulkanAPI::onNewFrame", "could not find a graphics queue");
            return;
        }

        auto queueSubmitResult = vkQueueSubmit(graphicsQueueOpt.value(),
            1, &submitInfo, vulkanRenderSyncs[currentFrame]->getFlightFence().getFence());
        Z_LOG_OBJ("VulkanAPI::onNewFrame::vkQueueSubmitResult", queueSubmitResult);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {
            vulkanSwapchain->getSwapchainKHR()
        };

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        // TODO: use presentation queue instead of graphics queue
        auto vkQueuePresentResult = vkQueuePresentKHR(graphicsQueueOpt.value(), &presentInfo);
        if (vkQueuePresentResult == VK_ERROR_OUT_OF_DATE_KHR ||
            vkQueuePresentResult == VK_SUBOPTIMAL_KHR
            || framebufferResized) {
            framebufferResized = false;
            recreateSwapchain();
        }
        else if (vkQueuePresentResult != VK_SUCCESS) {
            Z_LOG_TXT("VulkanAPI::onNewFrame", "failed to present swapchain image!");
            return;
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    Renderer& VulkanAPI::getRenderer() const {
        return *vulkanRenderer; 
    }

    ShaderAttributes& VulkanAPI::getShaderAttributes() const {
        return (ShaderAttributes&) shaderAttributes;
    }

    std::vector<std::shared_ptr<MagmaShader>>& VulkanAPI::getShaders() {
        return vulkanShaders;
    }

    std::shared_ptr<MagmaShader> VulkanAPI::createShader(const char* code, ShadercType type) {
        return createShader({ code, type });
    }

    std::shared_ptr<MagmaShader> VulkanAPI::createShader(VulkanShaderInfo _vkShaderInfo) {
        std::shared_ptr<MagmaShader> vulkanShader = std::make_shared<MagmaShader>(vulkanDevice);
        vulkanShader->pShaderCode = _vkShaderInfo.pCode;
        vulkanShader->pShaderType = (shaderc_shader_kind)_vkShaderInfo.pShaderType;
        vulkanShader->init();
        return vulkanShader;
    }

    std::shared_ptr<Buffer> VulkanAPI::createBuffer(int64_t size) {
        return createBuffer(size, BufferUsage::VERTEX);
    }

    std::shared_ptr<Buffer> VulkanAPI::createBuffer(int64_t size,
        BufferUsage bufferUsage) {
        std::shared_ptr<VulkanBuffer> vulkanBuffer = std::make_shared<VmaBuffer>(vulkanDevice);
        vulkanBuffer->pSize = (uint32_t)size;
        vulkanBuffer->pBufferUsageFlags = bufferUsage;
        vulkanBuffer->init();
        return vulkanBuffer;
    }

    void VulkanAPI::addBuffer(std::shared_ptr<Buffer> buffer) {
        vulkanRenderer->pVertexBuffers.push_back(dynamic_pointer_cast<VulkanBuffer>(buffer));
    }

    VkFormat VulkanAPI::findSupportedFormat(const std::vector<VkFormat>& candidates, 
        VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(vulkanDevice->getPhysicalDevice(), format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && 
                (props.linearTilingFeatures & features) == features)
                return format;
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && 
                (props.optimalTilingFeatures & features) == features)
                return format;
        }

        throw std::runtime_error("failed to find supported format!");
    }

    VkFormat VulkanAPI::findDepthFormat() {
        return findSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    void VulkanAPI::initCommands() {
        vulkanCmdPools.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            auto vulkanCmdPool = std::make_shared<VulkanCmdPool>(vulkanDevice);
            Z_LOG_OBJ("VulkanAPI::initCommands::vulkanCmdPool", vulkanCmdPool->init());
            vulkanCmdPools[i] = vulkanCmdPool;
        }

        vulkanCmdBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            auto vulkanCmdBuffer = std::make_shared<VulkanCmdBuffer>(vulkanDevice, vulkanCmdPools[i]);
            Z_LOG_OBJ("VulkanAPI::initCommands::vulkanCmdBuffer", vulkanCmdBuffer->init());
            vulkanCmdBuffers[i] = vulkanCmdBuffer;
        }
    }

    void VulkanAPI::initSync() {
        vulkanRenderSyncs.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            auto vulkanRenderSync = std::make_shared<VulkanRenderSync>();
            vulkanRenderSync->pDevice = &vulkanDevice->getDevice();
            Z_LOG_OBJ("VulkanAPI::initSync::vulkanRenderSync", vulkanRenderSync->init());
            vulkanRenderSyncs[i] = vulkanRenderSync;
        }
    }

    void VulkanAPI::recreateSwapchain() {
        // pause program while minimized
        auto framebufferSize = windowImpl.getFramebufferSize();
        while (framebufferSize.first == 0 || framebufferSize.second == 0) {
            framebufferSize = windowImpl.getFramebufferSize();
            windowImpl.waitForEvents();
        }
        // wait for the device to be idle
        vkDeviceWaitIdle(vulkanDevice->getDevice());
        // TODO: move destruction and constructor into reinitialization method
        // destroy existing pipeline
        vulkanPipeline->destroyPipeline();
        // delete framebuffers
        vulkanPipeline->destroyFramebuffers();
        // delete image views
        vulkanSwapchain->deleteImageViews();
        // recreate swapchain and image views
        vulkanSwapchain->init();
        // reinit framebuffers
        vulkanPipeline->init();
    }

    std::vector<const char*> VulkanAPI::getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (vulkanValidater->isValidationEnabled()) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

}