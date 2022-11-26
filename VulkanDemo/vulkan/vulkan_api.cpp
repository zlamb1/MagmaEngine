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
    // unwrap the deque
    vulkanDeque.unwrap();
    // destroy VkInstance
    vkDestroyInstance(vulkanInstance, nullptr);
}

void VulkanAPI::initSetup(GLFWwindow* glfwWindow) {
    this->glfwWindow = glfwWindow;

    if (vulkanValidater.isValidationEnabled()) {
        vulkanDebugger = new VulkanDebugger(vulkanInstance);
        vulkanDeque.addObject(vulkanDebugger);
    }

    initInstance();

    if (vulkanValidater.isValidationEnabled()) {
        vulkanDebugger->init();
    }

    // init member fields
    vulkanSurface = new VulkanSurface();
    vulkanDeque.addObject(vulkanSurface);
    vulkanDevice = new VulkanDevice();
    vulkanDeque.addObject(vulkanDevice);
    vulkanSwapchain = new VulkanSwapchain();
    vulkanDeque.addObject(vulkanSwapchain);
    vulkanPipeline = new VulkanPipeline();
    vulkanDeque.addObject(vulkanPipeline);

    // create fields
    vulkanSurface->pWindow = glfwWindow;
    vulkanSurface->pInstance = &vulkanInstance;
    vulkanSurface->init();

    vulkanDevice->pInstance = &vulkanInstance;
    vulkanDevice->pSurfaceKHR = &vulkanSurface->getSurfaceKHR();
    vulkanDevice->pValidater = &vulkanValidater;
    vulkanDevice->init();

    vulkanSwapchain->pWindow = glfwWindow;
    vulkanSwapchain->pDevice = vulkanDevice;
    vulkanSwapchain->pSurfaceKHR = &vulkanSurface->getSurfaceKHR();
    vulkanSwapchain->init();

    defaultVertexShader = createShaderHandle(defVertexShader, VulkanShaderType::VERTEX);
    defaultFragmentShader = createShaderHandle(defFragmentShader, VulkanShaderType::FRAGMENT);
}

void VulkanAPI::initRender() {
    vulkanPipeline->pDevice = vulkanDevice;
    vulkanPipeline->pSwapchain = vulkanSwapchain;
    
    if (vulkanShaderHandles.empty()) {
        VulkanLogger::instance().enqueueText("VulkanAPI::initRender", 
            "warning: no shaders provided, using defaults");
        vulkanPipeline->addShader(defaultVertexShader);
        vulkanPipeline->addShader(defaultFragmentShader);
    }

    for (auto _vkShaderHandle : vulkanShaderHandles) {
        vulkanPipeline->addShader(_vkShaderHandle);
    }

    for (auto _vkBufferHandle : vulkanBufferHandles) {
        vulkanPipeline->pBuffers.push_back(_vkBufferHandle);
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

    vulkanPipeline->onNewFrame(*vulkanCmdBuffers[currentFrame], imageIndex, vertexCount);

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

VulkanDevice* VulkanAPI::getVulkanDevice() {
    return vulkanDevice;
}

void VulkanAPI::addVertexInputState(VulkanVertexState& vertexState) {
    vulkanPipeline->pBindingDescriptions.push_back(vertexState.getBindingDescription());
    for (const auto& vulkanAttributeDescription : vertexState.getAttributeDescriptions()) {
        vulkanPipeline->pAttributeDescriptions.push_back(vulkanAttributeDescription);
    }
}

std::vector<VulkanShader*>& VulkanAPI::getShaderHandles() {
    return vulkanShaderHandles;
}

std::vector<VulkanBuffer*>& VulkanAPI::getBufferHandles() {
    return vulkanBufferHandles;
}

void VulkanAPI::setFramebufferResized(bool framebufferResized) {
    this->framebufferResized = framebufferResized;
}

VulkanShader* VulkanAPI::createShaderHandle(const char* code, VulkanShaderType type) {
    return createShaderHandle({ code, type });
}

VulkanShader* VulkanAPI::createShaderHandle(VulkanShaderInfo _vkShaderInfo) {
    VulkanShader* vulkanShaderHandle = new VulkanShader();
    vulkanShaderHandle->pDevice = &vulkanDevice->getDevice();
    vulkanShaderHandle->pShaderCode = _vkShaderInfo.pCode;
    vulkanShaderHandle->pShaderType = (shaderc_shader_kind)_vkShaderInfo.pShaderType;
    vulkanShaderHandle->init();
    vulkanDeque.addObject(vulkanShaderHandle);
    return vulkanShaderHandle;
}

VulkanBuffer* VulkanAPI::createBufferHandle(uint32_t pSize) {
    VulkanBuffer* vulkanBuffer = new VulkanBuffer();
    vulkanBuffer->pPhysicalDevice = &vulkanDevice->getPhysicalDevice();
    vulkanBuffer->pDevice = &vulkanDevice->getDevice();
    vulkanBuffer->pSize = pSize;
    VulkanLogger::instance().enqueueObject("VulkanAPI::createBufferHandle", vulkanBuffer->init());
    vulkanDeque.addObject(vulkanBuffer);
    return vulkanBuffer;
}

VulkanBuffer* VulkanAPI::createBufferHandle(VkDeviceSize pSize, VulkanBufferUsage pBufferUsage,
    VulkanMemoryType pMemType) {
    VulkanBuffer* vulkanBuffer = new VulkanBuffer();
    vulkanBuffer->pPhysicalDevice = &vulkanDevice->getPhysicalDevice();
    vulkanBuffer->pDevice = &vulkanDevice->getDevice();
    vulkanBuffer->pSize = (uint32_t) pSize;
    vulkanBuffer->pBufferUsageFlags = pBufferUsage;
    vulkanBuffer->pMemPropertyFlags = pMemType;
    VulkanLogger::instance().enqueueObject("VulkanAPI::createBufferHandle", vulkanBuffer->init());
    vulkanDeque.addObject(vulkanBuffer);
    return vulkanBuffer;
}

// Private Implementation

void VulkanAPI::initInstance() {
    if (vulkanValidater.isValidationEnabled() && !vulkanValidater.ensureRequiredLayers()) {
        // TODO: add check to see if layers are strictly necessary
        VulkanLogger::instance().enqueueText("VulkanAPI::initInstance",
            "Validation layers were requested but are not available");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanDemo";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (vulkanValidater.isValidationEnabled()) {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(vulkanValidater.getLayerSize());
        instanceCreateInfo.ppEnabledLayerNames = vulkanValidater.getLayerData();

        vulkanDebugger->PopulateDebugMessengerCreateInfo(debugCreateInfo);
        instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.pNext = nullptr;
    }

    auto createInstanceResult = vkCreateInstance(&instanceCreateInfo, nullptr, &vulkanInstance);
    switch (createInstanceResult) {
        case VK_ERROR_LAYER_NOT_PRESENT:
            VulkanLogger::instance().enqueueText("VulkanAPI::initInstance::vkCreateInstance", 
                "failed to find required layers");
            VulkanLogger::instance().enqueueText("VulkanAPI::initInstance", 
                "Trying vkCreateInstance again...");

            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.ppEnabledLayerNames = nullptr;

            // try to create instance again with no layers
            createInstanceResult = vkCreateInstance(&instanceCreateInfo, nullptr, &vulkanInstance);
            VulkanLogger::instance().enqueueObject("VulkanAPI::initInstance::vkCreateInstance", 
                createInstanceResult);
            break;
        case VK_SUCCESS:
            break;
        default:
            VulkanLogger::instance().enqueueObject("VulkanAPI::initInstance::vkCreateInstance", 
                createInstanceResult);
            break;
    }
}

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
    vulkanPipeline->deleteFramebuffers();
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
        auto vulkanCmdPool = new VulkanCmdPool();
        vulkanCmdPool->pDevice = &vulkanDevice->getDevice();
        vulkanCmdPool->pQueueFamily = &vulkanDevice->getQueueFamily();
        VulkanLogger::instance().enqueueObject("VulkanAPI::initCommands::vulkanCmdPool", 
            vulkanCmdPool->init());
        vulkanDeque.addObject(vulkanCmdPool);
        vulkanCmdPools[i] = vulkanCmdPool;
    }
    
    vulkanCmdBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto vulkanCmdBuffer = new VulkanCmdBuffer();
        vulkanCmdBuffer->pCmdPool = &vulkanCmdPools[i]->getCmdPool();
        vulkanCmdBuffer->pDevice = &vulkanDevice->getDevice();
        VulkanLogger::instance().enqueueObject("VulkanAPI::initCommands::vulkanCmdBuffer", 
            vulkanCmdBuffer->init());
        vulkanDeque.addObject(vulkanCmdBuffer);
        vulkanCmdBuffers[i] = vulkanCmdBuffer;
    }
}

void VulkanAPI::initSync() {
    vulkanRenderSyncs.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto vulkanRenderSync = new VulkanRenderSync();
        vulkanRenderSync->pDevice = &vulkanDevice->getDevice();
        VulkanLogger::instance().enqueueObject("VulkanAPI::initSync::vulkanRenderSync", 
            vulkanRenderSync->init());
        vulkanDeque.addObject(vulkanRenderSync);
        vulkanRenderSyncs[i] = vulkanRenderSync;
    }
}

std::vector<const char*> VulkanAPI::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (vulkanValidater.isValidationEnabled()) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}