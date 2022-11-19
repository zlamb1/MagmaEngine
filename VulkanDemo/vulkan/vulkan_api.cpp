#include "vulkan_api.h"

#ifdef NDEBUG
    const bool debugMode = false;
#else
    const bool debugMode = true;
#endif

// Public

VulkanAPI::VulkanAPI(GLFWwindow& _window) : 
    logger{ _VkLogger::Instance() },
    window{ _window }
{

}

VulkanAPI::~VulkanAPI() {

    // we need the wrappers to be deleted in a specific order
    delete vkSyncWrapper;

    delete vkCmdBufferWrapper;
    delete vkCmdPoolWrapper;

    delete vkPipelineWrapper;
    delete vkSwapChainWrapper;
    delete vkDeviceWrapper;

    delete vkSurfaceWrapper;

    delete vkDebugWrapper;

    vkDestroyInstance(vkInstance, nullptr);
}

void VulkanAPI::newFrame() {
    vkWaitForFences(vkDeviceWrapper->vkDevice, 1,
        &vkSyncWrapper->getFlightFence(), VK_TRUE, UINT64_MAX);
    vkResetFences(vkDeviceWrapper->vkDevice,
        1, &vkSyncWrapper->getFlightFence());

    uint32_t imageIndex;
    vkAcquireNextImageKHR(vkDeviceWrapper->vkDevice,
        vkSwapChainWrapper->getSwapchain(), UINT64_MAX,
        vkSyncWrapper->getImageSemaphore(), VK_NULL_HANDLE, &imageIndex);

    vkPipelineWrapper->newFrame(*vkCmdBufferWrapper, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { vkSyncWrapper->getImageSemaphore() };
    VkPipelineStageFlags waitStages[] = { 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT 
    };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCmdBufferWrapper->vkCmdBuffer;

    VkSemaphore signalSemaphores[] = { 
        vkSyncWrapper->getRenderSemaphore() 
    };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    auto vkQueueSubmitResult = vkQueueSubmit(vkDeviceWrapper->vkGraphicsQueue, 1,
        &submitInfo, vkSyncWrapper->getFlightFence());
    if (vkQueueSubmitResult != VK_SUCCESS) {
        logger.LogResult("vkQueueSubmitResult Error => ", vkQueueSubmitResult);
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { 
        vkSwapChainWrapper->getSwapchain() 
    };

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(vkDeviceWrapper->vkPresentationQueue, &presentInfo);

    // allows for clean exit 
    vkDeviceWaitIdle(vkDeviceWrapper->vkDevice);
}

void VulkanAPI::init() {
    if (vkValidationWrapper.enabled) {
        vkDebugWrapper = new VkDebugWrapper(vkInstance);
    }

    initInstance();

    if (vkValidationWrapper.enabled) {
        vkDebugWrapper->start();
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

    if (vkValidationWrapper.enabled && !vkValidationWrapper.ensureRequiredLayers()) {
        // TODO: add check to see if layers are strictly necessary
        logger.LogText("Validation layers were requested but are not available!");
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

    if (vkValidationWrapper.enabled) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(vkValidationWrapper.size());
        createInfo.ppEnabledLayerNames = vkValidationWrapper.data();

        vkDebugWrapper->populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    auto result = vkCreateInstance(&createInfo, nullptr, &vkInstance);
    switch (result) {
        case VK_ERROR_LAYER_NOT_PRESENT:
            logger.LogText("vkCreateInstance: failed to find required layers");
            logger.LogText("Trying vkCreateInstance again...");

            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;

            // try to create instance again with no layers
            result = vkCreateInstance(&createInfo, nullptr, &vkInstance);
            logger.LogResult("vkCreateInstance Error =>", result);
            break;
        case VK_SUCCESS:
            break;
        default:
            logger.LogResult("vkCreateInstance Error =>", result);
            break;
    }
}

void VulkanAPI::initSurface() {
    vkSurfaceWrapper = new VkSurfaceWrapper(vkInstance, window);
}

void VulkanAPI::initDevice() {
    vkDeviceWrapper = new VkDeviceWrapper();

    vkDeviceWrapper->pInstance = &vkInstance;
    vkDeviceWrapper->pSurfaceKHR = &vkSurfaceWrapper->getSurface();
    vkDeviceWrapper->pValidationWrapper = &vkValidationWrapper;

    auto vkDeviceCreateResult = vkDeviceWrapper->create();
}

void VulkanAPI::initSwapChain() {
    vkSwapChainWrapper = new VkSwapChainWrapper(
        window, *vkSurfaceWrapper, *vkDeviceWrapper);
}

void VulkanAPI::initPipeline() {
    vkPipelineWrapper = new VkPipelineWrapper(
        *vkDeviceWrapper, *vkSwapChainWrapper);
    vkPipelineWrapper->init();
}

void VulkanAPI::initCmdWrapper() {
    
    vkCmdPoolWrapper = new VkCmdPoolWrapper();
    vkCmdPoolWrapper->pDevice = &vkDeviceWrapper->vkDevice;
    vkCmdPoolWrapper->pQueueFamily = &vkDeviceWrapper->vkQueueFamily;

    auto vkCreateCommandPoolResult = vkCmdPoolWrapper->create();
    if (vkCreateCommandPoolResult != VK_SUCCESS) {
        logger.LogResult("vkCreateCommandPool =>", vkCreateCommandPoolResult);
    }

    vkCmdBufferWrapper = new VkCmdBufferWrapper();
    vkCmdBufferWrapper->pCmdPool = vkCmdPoolWrapper;
    vkCmdBufferWrapper->pDevice = &vkDeviceWrapper->vkDevice;

    auto vkAllocateCommandBuffersResult = vkCmdBufferWrapper->create();
    if (vkAllocateCommandBuffersResult != VK_SUCCESS) {
        logger.LogResult("vkAllocateCommandBuffers =>", vkAllocateCommandBuffersResult);
    }
}

void VulkanAPI::initSync() {
    vkSyncWrapper = new VkSyncWrapper(*vkDeviceWrapper);
    vkSyncWrapper->init();
}

std::vector<const char*> VulkanAPI::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (vkValidationWrapper.enabled) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}