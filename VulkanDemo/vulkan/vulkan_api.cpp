#include "vulkan_api.h"

// Public Implementation

VulkanAPI::VulkanAPI(GLFWwindow& _window) : 
    glfwWindow{ _window },
    _vkLogger{ _VkLogger::Instance() }
{

}

VulkanAPI::~VulkanAPI() {
    // allows for clean exit 
    vkDeviceWaitIdle(_vkDevice->vkDevice);
    // delete shaders
    for (auto _vkShaderHandle : _vkCreatedShaderHandles) {
        delete _vkShaderHandle;
    }
    // swapchain has custom lifetime
    delete _vkSwapchain;
    // unwrap the deque
    deque.unwrap();
    // debug wrapper isn't VulkanWrapper
    delete _vkDebugWrapper;
    // destroy VkInstance
    vkDestroyInstance(vkInstance, nullptr);
}

void VulkanAPI::initSetup() {
    if (_vkValidation.validationEnabled) {
        _vkDebugWrapper = new VkDebugWrapper(vkInstance);
    }

    initInstance();

    if (_vkValidation.validationEnabled) {
        _vkDebugWrapper->init();
    }

    initSurface();
    initDevice();
    createSwapchain();
}

void VulkanAPI::initRender() {
    initPipeline();
    initCmd();
    initSync();
}

void VulkanAPI::onNewFrame() {
    vkWaitForFences(_vkDevice->vkDevice, 1,
        &_vkRenderSyncs[currentFrame]->_vkFlightFence.vkFence, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    auto vkAcquireNextImageResult = vkAcquireNextImageKHR(_vkDevice->vkDevice,
        _vkSwapchain->vkSwapchainKHR, UINT64_MAX,
        _vkRenderSyncs[currentFrame]->_vkImageSemaphore.vkSemaphore, VK_NULL_HANDLE, &imageIndex);

    if (vkAcquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
        return;
    } else if (vkAcquireNextImageResult != VK_SUCCESS) {
        _vkLogger.LogText("VulkanAPI:onNewFrame => failed to acquire new swapchain image!");
        return;
    }

    vkResetFences(_vkDevice->vkDevice, 1, &_vkRenderSyncs[currentFrame]->_vkFlightFence.vkFence);

    _vkPipeline->onNewFrame(*_vkCmdBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { _vkRenderSyncs[currentFrame]->_vkImageSemaphore.vkSemaphore };
    VkPipelineStageFlags waitStages[] = { 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT 
    };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_vkCmdBuffers[currentFrame]->vkCmdBuffer;

    VkSemaphore signalSemaphores[] = { 
        _vkRenderSyncs[currentFrame]->_vkRenderSemaphore.vkSemaphore
    };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    auto vkQueueSubmitResult = vkQueueSubmit(_vkDevice->vkGraphicsQueue, 1,
        &submitInfo, _vkRenderSyncs[currentFrame]->_vkFlightFence.vkFence);
    _vkLogger.LogResult("vkQueueSubmitResult => ", vkQueueSubmitResult);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { 
        _vkSwapchain->vkSwapchainKHR
    };

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    auto vkQueuePresentResult = vkQueuePresentKHR(_vkDevice->vkPresentationQueue, &presentInfo);
    if (vkQueuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || 
        vkQueuePresentResult == VK_SUBOPTIMAL_KHR
        || framebufferResized) {
        framebufferResized = false;
        recreateSwapchain();
    }
    else if (vkQueuePresentResult != VK_SUCCESS) {
        _vkLogger.LogText("VulkanAPI:onNewFrame => failed to present swapchain image!");
        return;
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

_VkShader* VulkanAPI::createShaderHandle(const char* code, _ShaderType type) {
    return createShaderHandle({ code, type });
}

_VkShader* VulkanAPI::createShaderHandle(_VkShaderInfo _vkShaderInfo) {
    _VkShader* _vkShaderHandle = new _VkShader();
    _vkShaderHandle->pDevice = &_vkDevice->vkDevice;
    _vkShaderHandle->pShaderCode = _vkShaderInfo.pCode;
    _vkShaderHandle->pShaderType = (shaderc_shader_kind)_vkShaderInfo.pShaderType;
    if (_vkShaderHandle->create() == VK_SUCCESS) {
        _vkCreatedShaderHandles.push_back(_vkShaderHandle);
    }
    return _vkShaderHandle;
}

void VulkanAPI::setFramebufferResized(bool framebufferResized) {
    this->framebufferResized = framebufferResized;
}

void VulkanAPI::addShaderHandle(_VkShader* _vkShaderHandle) {
    _vkActiveShaderHandles.push_back(_vkShaderHandle);
}

// Private Implementation

void VulkanAPI::initInstance() {
    if (_vkValidation.validationEnabled && !_vkValidation.ensureRequiredLayers()) {
        // TODO: add check to see if layers are strictly necessary
        _vkLogger.LogText("Validation layers were requested but are not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanDemo";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (_vkValidation.validationEnabled) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(_vkValidation.size());
        createInfo.ppEnabledLayerNames = _vkValidation.data();

        _vkDebugWrapper->populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    auto vkCreateInstanceResult = vkCreateInstance(&createInfo, nullptr, &vkInstance);
    switch (vkCreateInstanceResult) {
        case VK_ERROR_LAYER_NOT_PRESENT:
            _vkLogger.LogText("vkCreateInstance => failed to find required layers");
            _vkLogger.LogText("Trying vkCreateInstance again...");

            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;

            // try to create instance again with no layers
            vkCreateInstanceResult = vkCreateInstance(&createInfo, nullptr, &vkInstance);
            _vkLogger.LogResult("vkCreateInstance =>", vkCreateInstanceResult);
            break;
        case VK_SUCCESS:
            break;
        default:
            _vkLogger.LogResult("vkCreateInstance =>", vkCreateInstanceResult);
            break;
    }
}

void VulkanAPI::initSurface() {
    _vkSurface = new _VkSurface();
    _vkSurface->pWindow = &glfwWindow;
    _vkSurface->pInstance = &vkInstance;
    auto vkSurfaceCreateResult = _vkSurface->create();
    deque.addWrapper(_vkSurface);
}

void VulkanAPI::initDevice() {
    _vkDevice = new _VkDevice();
    _vkDevice->pInstance = &vkInstance;
    _vkDevice->pSurfaceKHR = &_vkSurface->vkSurfaceKHR;
    _vkDevice->_pValidation = &_vkValidation;
    auto vkDeviceCreateResult = _vkDevice->create();
    deque.addWrapper(_vkDevice);
}

void VulkanAPI::createSwapchain() {
    _vkSwapchain = new _VkSwapchain();
    _vkSwapchain->pWindow = &glfwWindow;
    _vkSwapchain->_pDevice = _vkDevice;
    _vkSwapchain->pSurfaceKHR = &_vkSurface->vkSurfaceKHR;
    _vkSwapchain->create();
}

void VulkanAPI::recreateSwapchain() {
    // pause program while minimized
    int width = 0, height = 0;
    glfwGetFramebufferSize(&glfwWindow, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(&glfwWindow, &width, &height);
        glfwWaitEvents();
    }
    // wait for the device to be idle
    vkDeviceWaitIdle(_vkDevice->vkDevice);
    // delete framebuffers
    _vkPipeline->deleteFramebuffers();
    // delete image views
    _vkSwapchain->deleteImageViews();
    // recreate swapchain and image views
    _vkSwapchain->create();
    // reinit framebuffers
    _vkPipeline->initFramebuffers();
}

void VulkanAPI::initPipeline() {
    _vkPipeline = new _VkPipeline();
    _vkPipeline->_pDevice = _vkDevice;
    _vkPipeline->_pSwapchain = _vkSwapchain;

    for (auto _vkShaderHandle : _vkActiveShaderHandles) {
        _vkPipeline->addShader(_vkShaderHandle);
    }

    _vkPipeline->create();
    deque.addWrapper(_vkPipeline);
}

void VulkanAPI::initCmd() {
    _vkCmdPools.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto _vkCmdPool = new _VkCmdPool();
        _vkCmdPool->pDevice = &_vkDevice->vkDevice;
        _vkCmdPool->pQueueFamily = &_vkDevice->vkQueueFamily;
        auto vkCreateCommandPoolResult = _vkCmdPool->create();
        _vkLogger.LogResult("vkCreateCommandPool =>", vkCreateCommandPoolResult);
        deque.addWrapper(_vkCmdPool);
        _vkCmdPools[i] = _vkCmdPool;
    }
    
    _vkCmdBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto _vkCmdBuffer = new _VkCmdBuffer();
        _vkCmdBuffer->pCmdPool = _vkCmdPools[i];
        _vkCmdBuffer->pDevice = &_vkDevice->vkDevice;
        auto vkAllocateCommandBuffersResult = _vkCmdBuffer->create();
        _vkLogger.LogResult("vkAllocateCommandBuffers =>", vkAllocateCommandBuffersResult);
        deque.addWrapper(_vkCmdBuffer);
        _vkCmdBuffers[i] = _vkCmdBuffer;
    }
}

void VulkanAPI::initSync() {
    _vkRenderSyncs.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto _vkRenderSync = new _VkRenderSync();
        _vkRenderSync->pDevice = &_vkDevice->vkDevice;
        _vkLogger.LogResult("_VkRenderSync =>", _vkRenderSync->create());
        deque.addWrapper(_vkRenderSync);
        _vkRenderSyncs[i] = _vkRenderSync;
    }
}

std::vector<const char*> VulkanAPI::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (_vkValidation.validationEnabled) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}