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

VulkanAPI::~VulkanAPI() {
    // allows for clean exit 
    vkDeviceWaitIdle(vulkanDevice->getDevice());
}

void VulkanAPI::initSetup(GLFWwindow* glfwWindow) {
    this->glfwWindow = glfwWindow;
    
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
        VulkanLogger::instance().enqueueText("VulkanAPI::initInstance",
            "Validation layers were requested but are not available");
    }

    vulkanInstance->pRequiredExtensions = getRequiredExtensions();
    vulkanInstance->init();

    if (vulkanValidater->isValidationEnabled()) {
        vulkanDebugger->init();
    }

    // create fields
    vulkanSurface = std::make_shared<VulkanSurface>(glfwWindow, vulkanInstance);
    vulkanSurface->init();

    vulkanDevice = std::make_shared<VulkanDevice>(vulkanInstance, vulkanSurface, vulkanValidater);
    vulkanDevice->init();

    vulkanSwapchain = std::make_shared<VulkanSwapchain>(vulkanDevice);
    vulkanSwapchain->init();

    vulkanDrawer = std::make_shared<VulkanDrawer>();

    vulkanPipeline = std::make_shared<VulkanPipeline>(vulkanSwapchain);

    defaultVertexShader = createShaderHandle(defVertexShader, VulkanShaderType::VERTEX);
    defaultFragmentShader = createShaderHandle(defFragmentShader, VulkanShaderType::FRAGMENT);
}

void VulkanAPI::initRender() {
    vulkanPipeline->pVulkanDrawer = vulkanDrawer;
    
    if (vulkanShaders.empty()) {
        VulkanLogger::instance().enqueueText("VulkanAPI::initRender", 
            "warning: no shaders provided, using defaults");
        vulkanPipeline->addShader(defaultVertexShader);
        vulkanPipeline->addShader(defaultFragmentShader);
    }

    for (auto vulkanShader : vulkanShaders) {
        vulkanPipeline->addShader(vulkanShader);
    }

    for (auto vulkanBuffer : vulkanBuffers) {
        vulkanDrawer->pVertexBuffers.push_back(vulkanBuffer);
    }

    vulkanPipeline->init();

    initCommands();
    initSync();
}

void VulkanAPI::onNewFrame(uint32_t vertexCount) {
    vkWaitForFences(vulkanDevice->getDevice(), 1,
        &vulkanRenderSyncs[currentFrame]->getFlightFence().getFence(), VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    auto vkAcquireNextImageResult = vkAcquireNextImageKHR(vulkanDevice->getDevice(),
        vulkanSwapchain->getSwapchainKHR(), UINT64_MAX,
        vulkanRenderSyncs[currentFrame]->getImageSemaphore().getSemaphore(), VK_NULL_HANDLE, &imageIndex);

    if (vkAcquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
        return;
    } else if (vkAcquireNextImageResult != VK_SUCCESS) {
        VulkanLogger::instance().enqueueText("VulkanAPI::onNewFrame", 
            "failed to acquire a new swapchain image!");
        return;
    }

    vkResetFences(vulkanDevice->getDevice(), 1, 
        &vulkanRenderSyncs[currentFrame]->getFlightFence().getFence());

    vulkanDrawer->pVertexCount = vertexCount;
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

    auto queueSubmitResult = vkQueueSubmit(vulkanDevice->getGraphicsQueue(), 1,
        &submitInfo, vulkanRenderSyncs[currentFrame]->getFlightFence().getFence());
    VulkanLogger::instance().enqueueObject("VulkanAPI::onNewFrame::vkQueueSubmitResult", 
        queueSubmitResult);

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

    auto vkQueuePresentResult = vkQueuePresentKHR(vulkanDevice->getPresentationQueue(), &presentInfo);
    if (vkQueuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || 
        vkQueuePresentResult == VK_SUBOPTIMAL_KHR
        || framebufferResized) {
        framebufferResized = false;
        recreateSwapchain();
    }
    else if (vkQueuePresentResult != VK_SUCCESS) {
        VulkanLogger::instance().enqueueText("VulkanAPI::onNewFrame", 
            "failed to present swapchain image!");
        return;
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

std::shared_ptr<VulkanDevice> VulkanAPI::getVulkanDevice() {
    return vulkanDevice;
}

void VulkanAPI::addVertexInputState(VulkanVertexState& vertexState) {
    vulkanPipeline->pBindingDescriptions.push_back(vertexState.getBindingDescription());
    for (const auto& vulkanAttributeDescription : vertexState.getAttributeDescriptions()) {
        vulkanPipeline->pAttributeDescriptions.push_back(vulkanAttributeDescription);
    }
}

std::vector<std::shared_ptr<VulkanShader>>& VulkanAPI::getShaderHandles() {
    return vulkanShaders;
}

std::vector<std::shared_ptr<VulkanBuffer>>& VulkanAPI::getBufferHandles() {
    return vulkanBuffers;
}

void VulkanAPI::setFramebufferResized(bool framebufferResized) {
    this->framebufferResized = framebufferResized;
}

std::shared_ptr<VulkanShader> VulkanAPI::createShaderHandle(const char* code, VulkanShaderType type) {
    return createShaderHandle({ code, type });
}

std::shared_ptr<VulkanShader> VulkanAPI::createShaderHandle(VulkanShaderInfo _vkShaderInfo) {
    std::shared_ptr<VulkanShader> vulkanShader = std::make_shared<VulkanShader>(vulkanDevice);
    vulkanShader->pShaderCode = _vkShaderInfo.pCode;
    vulkanShader->pShaderType = (shaderc_shader_kind)_vkShaderInfo.pShaderType;
    vulkanShader->init();
    return vulkanShader;
}

std::shared_ptr<VulkanBuffer> VulkanAPI::createBufferHandle(VkDeviceSize pSize) {
    return createBufferHandle(pSize, VulkanBufferUsage::VERTEX, VulkanMemoryType::CPU_VISIBLE |
        VulkanMemoryType::FLUSH_WRITES);
}

std::shared_ptr<VulkanBuffer> VulkanAPI::createBufferHandle(VkDeviceSize pSize, VulkanBufferUsage pBufferUsage,
    VulkanMemoryType pMemType) {
    std::shared_ptr<VulkanBuffer> vulkanBuffer = std::make_shared<VulkanBuffer>(vulkanDevice);
    vulkanBuffer->pSize = (uint32_t) pSize;
    vulkanBuffer->pBufferUsageFlags = pBufferUsage;
    vulkanBuffer->pMemPropertyFlags = pMemType;
    VulkanLogger::instance().enqueueObject("VulkanAPI::createBufferHandle", vulkanBuffer->init());
    return vulkanBuffer;
}

// Private Implementation

void VulkanAPI::recreateSwapchain() {
    // pause program while minimized
    int width = 0, height = 0;
    glfwGetFramebufferSize(glfwWindow, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(glfwWindow, &width, &height);
        glfwWaitEvents();
    }
    // wait for the device to be idle
    vkDeviceWaitIdle(vulkanDevice->getDevice());
    // delete framebuffers
    vulkanPipeline->destroyFramebuffers();
    // delete image views
    vulkanSwapchain->deleteImageViews();
    // recreate swapchain and image views
    vulkanSwapchain->init();
    // reinit framebuffers
    vulkanPipeline->initFramebuffers();
}

void VulkanAPI::initCommands() {
    vulkanCmdPools.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto vulkanCmdPool = std::make_shared<VulkanCmdPool>(vulkanDevice);
        VulkanLogger::instance().enqueueObject("VulkanAPI::initCommands::vulkanCmdPool", 
            vulkanCmdPool->init());
        vulkanCmdPools[i] = vulkanCmdPool;
    }
    
    vulkanCmdBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto vulkanCmdBuffer = std::make_shared<VulkanCmdBuffer>(vulkanDevice, vulkanCmdPools[i]);
        VulkanLogger::instance().enqueueObject("VulkanAPI::initCommands::vulkanCmdBuffer", 
            vulkanCmdBuffer->init());
        vulkanCmdBuffers[i] = vulkanCmdBuffer;
    }
}

void VulkanAPI::initSync() {
    vulkanRenderSyncs.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto vulkanRenderSync = std::make_shared<VulkanRenderSync>();
        vulkanRenderSync->pDevice = &vulkanDevice->getDevice();
        VulkanLogger::instance().enqueueObject("VulkanAPI::initSync::vulkanRenderSync", 
            vulkanRenderSync->init());
        vulkanRenderSyncs[i] = vulkanRenderSync;
    }
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