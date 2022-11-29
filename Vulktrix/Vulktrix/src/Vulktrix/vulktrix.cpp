#include "vulktrix.h"

const char* defVertexShader = R"(#version 450
    void main() {
        gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    })";

const char* defFragmentShader = R"(#version 450
    layout(location = 0) out vec3 fragColor;
    void main() {
        fragColor = vec3(1, 1, 1);
    })";

VulktrixAPI::VulktrixAPI(Window::VulkanImpl& windowImpl) : windowImpl{ windowImpl } {

    // add framebuffer resized callback
    windowImpl.addFramebufferSizeCallback([&](int32_t w, int32_t h) {
        setFramebufferResized(true);
    });

}

VulktrixAPI::~VulktrixAPI() {
    // allows for clean exit 
    vkDeviceWaitIdle(vulkanDevice->getDevice());
}

void VulktrixAPI::initSetup() {
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

    vulkanDrawer = std::make_shared<VulkanDrawer>();

    vulkanPipeline = std::make_shared<VulkanPipeline>(shaderAttributes);

    defaultVertexShader = createVulkanShader(defVertexShader, ShadercType::VERTEX);
    defaultFragmentShader = createVulkanShader(defFragmentShader, ShadercType::FRAGMENT);
}

void VulktrixAPI::initRender() {
    vulkanPipeline->pVulkanSwapchain = vulkanSwapchain;
    vulkanPipeline->pVulkanDrawer = vulkanDrawer;
    
    if (vulkanShaders.empty()) {
        Z_LOG_TXT("VulkanAPI::initRender", "warning: no shaders provided, using defaults");
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

void VulktrixAPI::onNewFrame(uint32_t vertexCount) {
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
        Z_LOG_TXT("VulkanAPI::onNewFrame", "failed to acquire a new swapchain image!");
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

std::shared_ptr<VulkanDevice> VulktrixAPI::getVulkanDevice() {
    return vulkanDevice;
}

std::shared_ptr<VulkanDrawer> VulktrixAPI::getVulkanDrawer() {
    return vulkanDrawer;
}

std::vector<std::shared_ptr<VulkanShader>>& VulktrixAPI::getVulkanShaders() {
    return vulkanShaders;
}

std::vector<std::shared_ptr<VulkanBuffer>>& VulktrixAPI::getVulkanBuffers() {
    return vulkanBuffers;
}

void VulktrixAPI::setFramebufferResized(bool framebufferResized) {
    this->framebufferResized = framebufferResized;
}

std::shared_ptr<VulkanShader> VulktrixAPI::createVulkanShader(const char* code, ShadercType type) {
    return createVulkanShader({ code, type });
}

std::shared_ptr<VulkanShader> VulktrixAPI::createVulkanShader(VulkanShaderInfo _vkShaderInfo) {
    std::shared_ptr<VulkanShader> vulkanShader = std::make_shared<VulkanShader>(vulkanDevice);
    vulkanShader->pShaderCode = _vkShaderInfo.pCode;
    vulkanShader->pShaderType = (shaderc_shader_kind)_vkShaderInfo.pShaderType;
    vulkanShader->init();
    return vulkanShader;
}

std::shared_ptr<VulkanBuffer> VulktrixAPI::createVulkanBuffer(VkDeviceSize pSize) {
    return createVulkanBuffer(pSize, VulkanBufferUsage::VERTEX);
}

std::shared_ptr<VulkanBuffer> VulktrixAPI::createVulkanBuffer(VkDeviceSize pSize,
    VulkanBufferUsage pBufferUsage) {
    std::shared_ptr<VulkanBuffer> vulkanBuffer = std::make_shared<VulkanBuffer>(vulkanDevice);
    vulkanBuffer->pSize = (uint32_t) pSize;
    vulkanBuffer->pBufferUsageFlags = pBufferUsage;
    Z_LOG_OBJ("VulkanAPI::createBufferHandle", vulkanBuffer->init());
    return vulkanBuffer;
}

std::shared_ptr<VulkanDeviceMemory> VulktrixAPI::createDeviceMemory(
    std::shared_ptr<VulkanBuffer> expectedBufferSpec, VulkanMemoryType pMemType) {
    std::shared_ptr<VulkanDeviceMemory> vulkanDeviceMemory = std::make_shared<VulkanDeviceMemory>(
        vulkanDevice, expectedBufferSpec);
    vulkanDeviceMemory->pMemPropertyFlags = pMemType;
    vulkanDeviceMemory->init();
    return vulkanDeviceMemory;
}

ShaderAttributes& VulktrixAPI::getShaderAttributes() {
    return shaderAttributes;
}

// Private

void VulktrixAPI::recreateSwapchain() {
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

void VulktrixAPI::initCommands() {
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

void VulktrixAPI::initSync() {
    vulkanRenderSyncs.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto vulkanRenderSync = std::make_shared<VulkanRenderSync>();
        vulkanRenderSync->pDevice = &vulkanDevice->getDevice();
        Z_LOG_OBJ("VulkanAPI::initSync::vulkanRenderSync", vulkanRenderSync->init());
        vulkanRenderSyncs[i] = vulkanRenderSync;
    }
}

std::vector<const char*> VulktrixAPI::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (vulkanValidater->isValidationEnabled()) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}