#include "vulkan_api.h"

#ifdef NDEBUG
    const bool debugMode = false;
#else
    const bool debugMode = true;
#endif

// Public

VulkanAPI::VulkanAPI(GLFWwindow& _window) : 
    glfwWindow{ _window },
    _vkLogger{ _VkLogger::Instance() }
{

}

VulkanAPI::~VulkanAPI() {

    // the wrappers need to be deleted in a specific order

    delete _vkRenderSync;

    delete _vkCmdBuffer;
    delete _vkCmdPool;

    delete _vkPipelineWrapper;
    delete _vkSwapChainWrapper;
    delete _vkDevice;

    delete _vkSurfaceWrapper;

    delete _vkDebugWrapper;

    vkDestroyInstance(vkInstance, nullptr);
}

void VulkanAPI::onNewFrame() {
    vkWaitForFences(_vkDevice->vkDevice, 1,
        &_vkRenderSync->_vkFlightFence.vkFence, VK_TRUE, UINT64_MAX);
    vkResetFences(_vkDevice->vkDevice,
        1, &_vkRenderSync->_vkFlightFence.vkFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(_vkDevice->vkDevice,
        _vkSwapChainWrapper->getSwapchain(), UINT64_MAX,
        _vkRenderSync->_vkImageSemaphore.vkSemaphore, VK_NULL_HANDLE, &imageIndex);

    _vkPipelineWrapper->newFrame(*_vkCmdBuffer, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { _vkRenderSync->_vkImageSemaphore.vkSemaphore };
    VkPipelineStageFlags waitStages[] = { 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT 
    };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_vkCmdBuffer->vkCmdBuffer;

    VkSemaphore signalSemaphores[] = { 
        _vkRenderSync->_vkRenderSemaphore.vkSemaphore
    };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    auto vkQueueSubmitResult = vkQueueSubmit(_vkDevice->vkGraphicsQueue, 1,
        &submitInfo, _vkRenderSync->_vkFlightFence.vkFence);
    _vkLogger.LogResult("vkQueueSubmitResult => ", vkQueueSubmitResult);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { 
        _vkSwapChainWrapper->getSwapchain() 
    };

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(_vkDevice->vkPresentationQueue, &presentInfo);

    // allows for clean exit 
    vkDeviceWaitIdle(_vkDevice->vkDevice);
}

void VulkanAPI::init() {
    if (_vkValidation.vkValidationEnabled) {
        _vkDebugWrapper = new VkDebugWrapper(vkInstance);
    }

    initInstance();

    if (_vkValidation.vkValidationEnabled) {
        _vkDebugWrapper->start();
    }

    initSurface();
    initDevice();
    initSwapChain();
    initPipeline();
    initCmdWrapper();
    initSync();
}

// Private

void VulkanAPI::initInstance() {

    if (_vkValidation.vkValidationEnabled && !_vkValidation.ensureRequiredLayers()) {
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

    if (_vkValidation.vkValidationEnabled) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(_vkValidation.size());
        createInfo.ppEnabledLayerNames = _vkValidation.data();

        _vkDebugWrapper->populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    auto result = vkCreateInstance(&createInfo, nullptr, &vkInstance);
    switch (result) {
        case VK_ERROR_LAYER_NOT_PRESENT:
            _vkLogger.LogText("vkCreateInstance => failed to find required layers");
            _vkLogger.LogText("Trying vkCreateInstance again...");

            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;

            // try to create instance again with no layers
            result = vkCreateInstance(&createInfo, nullptr, &vkInstance);
            _vkLogger.LogResult("vkCreateInstance =>", result);
            break;
        case VK_SUCCESS:
            break;
        default:
            _vkLogger.LogResult("vkCreateInstance =>", result);
            break;
    }
}

void VulkanAPI::initSurface() {
    _vkSurfaceWrapper = new VkSurfaceWrapper(glfwWindow, vkInstance);
    _vkSurfaceWrapper->Initialize();
}

void VulkanAPI::initDevice() {
    _vkDevice = new _VkDevice();

    _vkDevice->pInstance = &vkInstance;
    _vkDevice->pSurfaceKHR = &_vkSurfaceWrapper->GetSurfaceKHR();
    _vkDevice->_pValidation = &_vkValidation;

    auto vkDeviceCreateResult = _vkDevice->create();
}

void VulkanAPI::initSwapChain() {
    _vkSwapChainWrapper = new VkSwapChainWrapper(
        glfwWindow, *_vkSurfaceWrapper, *_vkDevice);
}

void VulkanAPI::initPipeline() {
    _vkPipelineWrapper = new VkPipelineWrapper(
        *_vkDevice, *_vkSwapChainWrapper);
    _vkPipelineWrapper->init();
}

void VulkanAPI::initCmdWrapper() {
    
    _vkCmdPool = new _VkCmdPool();
    _vkCmdPool->pDevice = &_vkDevice->vkDevice;
    _vkCmdPool->pQueueFamily = &_vkDevice->vkQueueFamily;

    auto vkCreateCommandPoolResult = _vkCmdPool->create();
    _vkLogger.LogResult("vkCreateCommandPool =>", vkCreateCommandPoolResult);

    _vkCmdBuffer = new _VkCmdBuffer();
    _vkCmdBuffer->pCmdPool = _vkCmdPool;
    _vkCmdBuffer->pDevice = &_vkDevice->vkDevice;

    auto vkAllocateCommandBuffersResult = _vkCmdBuffer->create();
    _vkLogger.LogResult("vkAllocateCommandBuffers =>", vkAllocateCommandBuffersResult);
}

void VulkanAPI::initSync() {
    _vkRenderSync = new _VkRenderSync();

    _vkRenderSync->pDevice = &_vkDevice->vkDevice;

    _vkLogger.LogResult("_VkRenderSync =>", _vkRenderSync->create());
}

std::vector<const char*> VulkanAPI::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (_vkValidation.vkValidationEnabled) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}