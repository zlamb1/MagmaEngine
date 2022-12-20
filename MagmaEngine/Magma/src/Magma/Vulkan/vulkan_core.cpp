#include "vulkan_core.h"

#include <GLFW/glfw3.h>

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

    VulkanCore::VulkanCore(VulkanImpl& windowImpl) : m_WindowImpl{ windowImpl } {
        // add framebuffer resized callback
        m_WindowImpl.addFramebufferSizeCallback([&](int32_t w, int32_t h) {
            m_FramebufferResized = true; 
        });
    }

    VulkanCore::~VulkanCore() {
        // allows for clean exit 
        vkDeviceWaitIdle(m_Device->getDevice());
    }

    void VulkanCore::init() {
        m_Validater = std::make_shared<Validater>();
        m_Instance = std::make_shared<Instance>(m_Validater);

        if (m_Validater->isValidationEnabled()) {
            m_Debugger = std::make_shared<Debugger>(m_Instance);
            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            m_Debugger->populateDebugMessengerCreateInfo(debugCreateInfo);
            m_Instance->m_DebugCreateInfo = debugCreateInfo;
        }

        if (m_Validater->isValidationEnabled() && !m_Validater->ensureRequiredLayers()) {
            // TODO: add check to see if layers are strictly necessary
            Z_LOG_TXT("VulkanAPI::initInstance", "Validation layers were requested but are not available");
        }

        m_Instance->m_RequiredExtensions = getRequiredExtensions();
        m_Instance->init();

        if (m_Validater->isValidationEnabled()) {
            m_Debugger->init();
        }

        // create fields
        m_Surface = std::make_shared<Surface>(m_WindowImpl, m_Instance);
        m_Surface->init();

        m_Device = std::make_shared<VulkanDevice>(m_Instance, m_Surface, m_Validater);
        m_Device->init();

        m_ShaderAttributes.m_Device = m_Device;

        m_Swapchain = std::make_shared<Swapchain>(m_Device);
        m_Swapchain->init();

        m_Renderer = std::make_shared<VulkanRenderer>();

        m_Pipeline = std::make_shared<VulkanPipeline>(m_ShaderAttributes);

        m_VertexShader = createShader(defVertexShader, ShadercType::VERTEX);
        m_FragmentShader = createShader(defFragmentShader, ShadercType::FRAGMENT);
    }

    void VulkanCore::initRender() {
        m_Renderer->m_DescriptorSets = m_ShaderAttributes.getDescriptorSets();

        m_Pipeline->pVulkanSwapchain = m_Swapchain;
        m_Pipeline->pVulkanRenderer = m_Renderer;

        if (m_Shaders.empty()) {
            Z_LOG_TXT("VulkanAPI::initRender", "warning: no shaders provided, using defaults");
            m_Pipeline->addShader(m_VertexShader);
            m_Pipeline->addShader(m_FragmentShader);
        }

        for (const auto& shader : m_Shaders) {
            m_Pipeline->addShader(shader);
        }

        for (const auto& buffer : m_Buffers) {
            m_Renderer->m_VertexBuffers.push_back(buffer);
        }

        m_Pipeline->init();

        initCommands();
        initSync();
    }

    void VulkanCore::onNewFrame() {
        vkWaitForFences(m_Device->getDevice(), 1,
            &m_RenderSyncs[m_CurrentFrame]->getFlightFence().getFence(), VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        const auto vkAcquireNextImageResult = vkAcquireNextImageKHR(m_Device->getDevice(),
            m_Swapchain->getSwapchainKHR(), UINT64_MAX,
            m_RenderSyncs[m_CurrentFrame]->getImageSemaphore().getSemaphore(), VK_NULL_HANDLE, &imageIndex);

        if (vkAcquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapchain();
            return;
        }
        else if (vkAcquireNextImageResult != VK_SUCCESS) {
            Z_LOG_TXT("VulkanAPI::onNewFrame", "failed to acquire a new swapchain image!");
            return;
        }

        vkResetFences(m_Device->getDevice(), 1,
            &m_RenderSyncs[m_CurrentFrame]->getFlightFence().getFence());

        m_Pipeline->onNewFrame(*m_CommandBuffers[m_CurrentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        const VkSemaphore waitSemaphores[] = { m_RenderSyncs[m_CurrentFrame]->getImageSemaphore().getSemaphore() };
        constexpr VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame]->getCmdBuffer();

        const VkSemaphore signalSemaphores[] = {
            m_RenderSyncs[m_CurrentFrame]->getRenderSemaphore().getSemaphore()
        };

        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        const auto graphicsQueueOpt = DeviceProfile::getQueue(VulkanQueueType::GRAPHICS);
        if (!graphicsQueueOpt.has_value()) {
            Z_LOG_TXT("VulkanAPI::onNewFrame", "could not find a graphics queue");
            return;
        }

        auto queueSubmitResult = vkQueueSubmit(graphicsQueueOpt.value(),
            1, &submitInfo, m_RenderSyncs[m_CurrentFrame]->getFlightFence().getFence());
        Z_LOG_OBJ("VulkanAPI::onNewFrame::vkQueueSubmitResult", queueSubmitResult);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        const VkSwapchainKHR swapChains[] = {
            m_Swapchain->getSwapchainKHR()
        };

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        // TODO: use presentation queue instead of graphics queue
        const auto vkQueuePresentResult = vkQueuePresentKHR(graphicsQueueOpt.value(), &presentInfo);
        if (vkQueuePresentResult == VK_ERROR_OUT_OF_DATE_KHR ||
            vkQueuePresentResult == VK_SUBOPTIMAL_KHR
            || m_FramebufferResized) {
            m_FramebufferResized = false;
            recreateSwapchain();
        }
        else if (vkQueuePresentResult != VK_SUCCESS) {
            Z_LOG_TXT("VulkanAPI::onNewFrame", "failed to present swapchain image!");
            return;
        }

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    std::shared_ptr<VulkanDevice> VulkanCore::getDevice() {
        return m_Device;
    }

    Renderer& VulkanCore::getRenderer() const {
        return *m_Renderer; 
    }

    ShaderAttributes& VulkanCore::getShaderAttributes() {
        return m_ShaderAttributes;
    }

    std::vector<std::shared_ptr<Shader>>& VulkanCore::getShaders() {
        return m_Shaders;
    }

    std::shared_ptr<Shader> VulkanCore::createShader(const char* code, ShadercType type) {
        return createShader({ code, type });
    }

    std::shared_ptr<Shader> VulkanCore::createShader(VulkanShaderInfo shaderInfo) {
        auto shader = std::make_shared<Shader>(m_Device);
        shader->m_ShaderCode = shaderInfo.pCode;
        shader->m_ShaderType = static_cast<shaderc_shader_kind>(shaderInfo.pShaderType);
        shader->init();
        return shader;
    }

    std::shared_ptr<Buffer> VulkanCore::createBuffer(int64_t size) {
        return createBuffer(size, BufferUsage::VERTEX);
    }

    std::shared_ptr<Buffer> VulkanCore::createBuffer(int64_t size,
        BufferUsage bufferUsage) {
        std::shared_ptr<VulkanBuffer> vulkanBuffer = std::make_shared<VmaBuffer>(m_Device);
        vulkanBuffer->pSize = static_cast<uint32_t>(size);
        vulkanBuffer->pBufferUsageFlags = bufferUsage;
        vulkanBuffer->init();
        return vulkanBuffer;
    }

    std::shared_ptr<Image> VulkanCore::createTexture(uint32_t width, uint32_t height) {
        auto image = std::make_shared<VulkanImage>(m_Device);
        image->m_ExtentWidth = width;
        image->m_ExtentHeight = height;
        image->m_InitialLayout = ImageLayout::UNDEFINED;
        image->m_ImageUsage = static_cast<VkImageUsageFlagBits>(VK_IMAGE_USAGE_SAMPLED_BIT | 
            VK_IMAGE_USAGE_TRANSFER_DST_BIT); 
        image->init();
        return image;
    }

    std::shared_ptr<VulkanImageView> VulkanCore::createTextureImageView(const std::shared_ptr<Image> image) {
        auto _image = std::dynamic_pointer_cast<VulkanImage>(image); 
        auto imageView = std::make_shared<VulkanImageView>(m_Device, _image); 
        imageView->init();
        return imageView; 
    }

    std::shared_ptr<Sampler> VulkanCore::createSampler() const {
        std::shared_ptr<Sampler> sampler = std::make_shared<Sampler>();
        sampler->pDevice = m_Device; 
        sampler->init();
        return sampler; 
    }

    void VulkanCore::addBuffer(std::shared_ptr<Buffer> buffer) {
        m_Renderer->m_VertexBuffers.push_back(dynamic_pointer_cast<VulkanBuffer>(buffer));
    }

    void VulkanCore::setDepthBuffering(bool enabled) {
        if (m_DepthImage != nullptr) m_DepthImage.reset();
        if (m_DepthImageView != nullptr) m_DepthImageView.reset();
        if (enabled) {
            const auto& [width, height] = m_Swapchain->getSwapchainExtent();
            m_DepthImage = std::make_shared<VulkanImage>(m_Device);
            m_DepthImage->m_ExtentWidth = width;
            m_DepthImage->m_ExtentHeight = height;

            const auto depthFormat = findDepthFormat();
            m_DepthImage->m_Format = static_cast<DataFormat>(depthFormat);
            m_DepthImage->m_ImageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; 
            m_DepthImage->init();

            m_DepthImageView = std::make_shared<VulkanImageView>(m_Device, m_DepthImage);
            m_DepthImageView->pImageAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
            m_DepthImageView->init();

            m_Pipeline->pDepthImageView = m_DepthImageView;
        }
    }

    VkFormat VulkanCore::findSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling, VkFormatFeatureFlags features) const {
        for (const VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_Device->getPhysicalDevice(), format, &props);
            if ((tiling == VK_IMAGE_TILING_LINEAR && 
                (props.linearTilingFeatures & features) == features) || (tiling == VK_IMAGE_TILING_OPTIMAL &&
                (props.optimalTilingFeatures & features) == features)) {
                return format;
            }
        }

        // TODO: handle without throwing exception
        throw std::runtime_error("failed to find supported format!");
    }

    VkFormat VulkanCore::findDepthFormat() const {
        return findSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    void VulkanCore::initCommands() {
        m_CommandPools.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            const auto commandPool = std::make_shared<VulkanCmdPool>(m_Device);
            Z_LOG_OBJ("VulkanAPI::initCommands::VkCommandPool", commandPool->init());
            m_CommandPools[i] = commandPool;
        }

        m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            const auto commandBuffer = std::make_shared<VulkanCmdBuffer>(m_Device, m_CommandPools[i]);
            Z_LOG_OBJ("VulkanAPI::initCommands::VkCommandBuffer", commandBuffer->init());
            m_CommandBuffers[i] = commandBuffer;
        }
    }

    void VulkanCore::initSync() {
        m_RenderSyncs.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            const auto renderSync = std::make_shared<VulkanRenderSync>();
            renderSync->pDevice = &m_Device->getDevice();
            Z_LOG_OBJ("VulkanAPI::initSync", renderSync->init());
            m_RenderSyncs[i] = renderSync;
        }
    }

    void VulkanCore::recreateSwapchain() {
        // pause program while minimized
        auto [width, height] = m_WindowImpl.getFramebufferSize();
        while (width <= 0 || height <= 0) {
        	std::tie(width, height) = m_WindowImpl.getFramebufferSize();
            m_WindowImpl.waitForEvents();
        }
        // wait for the device to be idle
        vkDeviceWaitIdle(m_Device->getDevice());
        // TODO: move destruction and constructor into reinitialization method
        // destroy existing pipeline
        m_Pipeline->destroyPipeline();
        // delete framebuffers
        m_Pipeline->destroyFramebuffers();
        // delete image views
        m_Swapchain->deleteImageViews();
        // recreate swapchain and image views
        m_Swapchain->init();
        // reset depth buffering state
        setDepthBuffering(true);
        // reinit framebuffers
        m_Pipeline->init();
    }

    std::vector<const char*> VulkanCore::getRequiredExtensions() const {
        // TODO: abstract this?
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (m_Validater->isValidationEnabled()) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

}