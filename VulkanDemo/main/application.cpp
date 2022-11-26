#include "application.h"

namespace MapUtility {
    static float map(float in, float min, float max, float dMin, float dMax) {
        float slope = 1.0f * (dMax - dMin) / (max - min);
        return dMin + slope * (in - min);
    }
}

class MyVertexState : public VulkanVertexState {

public:
    VkVertexInputBindingDescription getBindingDescription() override {
        pBindingDescription.binding = 0;
        pBindingDescription.stride = sizeof(float) * 5;
        return pBindingDescription.getBindingDescription();
    }
    
    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() override {
        pAttributeDescriptions.resize(2);
        pAttributeDescriptions[0].binding = 0;
        pAttributeDescriptions[0].location = 0;
        pAttributeDescriptions[0].format = VulkanFormat::R32G32_SFLOAT;
        pAttributeDescriptions[0].offset = 0;
        pAttributeDescriptions[1].binding = 0;
        pAttributeDescriptions[1].location = 1;
        pAttributeDescriptions[1].format = VulkanFormat::R32G32B32_SFLOAT;
        pAttributeDescriptions[1].offset = sizeof(float) * 2;
        return VulkanVertexState::getAttributeDescriptions();
    }
};

std::vector<float> vertex_data = {
    -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.5f, 0.5f
};

std::vector<uint16_t> index_data{ 0, 1, 2, 2, 3, 0 };

const char* vertexShaderCode = R"(#version 450
    layout(location = 0) in vec2 vertexPos;
    layout(location = 1) in vec3 vertexColor;
    layout(location = 0) out vec3 fragColor;

    void main() {
        gl_Position = vec4(vertexPos, 0.0, 1.0);
        fragColor = vertexColor;
    })";

const char* fragmentShaderCode = R"(#version 450
    layout(location = 0) in vec3 fragColor;
    layout(location = 0) out vec4 outColor;
    void main() {
        outColor = vec4(fragColor, 1.0);
    })";

Application& Application::instance() {
    static Application app;
    return app;
}

int Application::run() {

    int exit_status = EXIT_SUCCESS;

    try {
        initWindow();
        initVulkan();
        mainLoop();
    }
    catch (const std::exception& error) {
        std::cout << error.what() << std::endl;
        exit_status = EXIT_FAILURE;
    }   

    system("pause");
    return exit_status;
}

// Private

Application::~Application() {
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

void Application::initWindow() {
    // init
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    // set callbacks
    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, onFramebufferResize);

    // set window pos
    const GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwSetWindowPos(glfwWindow, mode->width - WIDTH, 30);
}

void Application::initVulkan() {
    vulkanAPI.initSetup(glfwWindow);

    auto vertexShader = vulkanAPI.createShaderHandle(vertexShaderCode, VulkanShaderType::VERTEX);
    vulkanAPI.getShaderHandles().push_back(vertexShader);
    auto fragmentShader = vulkanAPI.createShaderHandle(fragmentShaderCode, VulkanShaderType::FRAGMENT);
    vulkanAPI.getShaderHandles().push_back(fragmentShader);
    
    auto myVertex = MyVertexState{};
    vulkanAPI.addVertexInputState(myVertex);
    
    auto bufferSize = sizeof(float) * vertex_data.size();

    stagingBuffer = vulkanAPI.createBufferHandle(bufferSize, VulkanBufferUsage::TRANSFER_SRC, 
        VulkanMemoryType::CPU_VISIBLE | VulkanMemoryType::FLUSH_WRITES);

    stagingBuffer->setData(vertex_data.data());

    vertexBuffer = vulkanAPI.createBufferHandle(bufferSize,
        VulkanBufferUsage::TRANSFER_DST | VulkanBufferUsage::VERTEX,
        VulkanMemoryType::GPU_EFFICIENT);

    BufferCopy::copyBuffer(vulkanAPI.getVulkanDevice(), stagingBuffer->pSize,
        stagingBuffer->getBuffer(), vertexBuffer->getBuffer(), 0, 0);

    vulkanAPI.getBufferHandles().push_back(vertexBuffer);

    stagingBuffer->setData(index_data.data());

    indexBuffer = vulkanAPI.createBufferHandle(sizeof(uint16_t) * index_data.size(),
        VulkanBufferUsage::TRANSFER_DST | VulkanBufferUsage::INDEX, VulkanMemoryType::GPU_EFFICIENT);
    BufferCopy::copyBuffer(vulkanAPI.getVulkanDevice(), indexBuffer->pSize, stagingBuffer->getBuffer(),
        indexBuffer->getBuffer(), 0, 0);

    vulkanAPI.getVulkanDrawer()->pIndexBuffer = indexBuffer;
    vulkanAPI.getVulkanDrawer()->pIndexCount = 6; 
    vulkanAPI.getVulkanDrawer()->pUseIndexing = true;

    vulkanAPI.initRender();
}

void Application::mainLoop() {
    while (!glfwWindowShouldClose(glfwWindow)) {
        vertex_data[2] = MapUtility::map(sin(x), -1.0f, 1.0f, 0.0f, 1.0f);
        vertex_data[8] = MapUtility::map(sin(x + 1.0f), -1.0f, 1.0f, 0.0f, 1.0f);
        vertex_data[14] = MapUtility::map(sin(x + 2.0f), -1.0f, 1.0f, 0.0f, 1.0f);
        stagingBuffer->setData(vertex_data.data());

        BufferCopy::copyBuffer(vulkanAPI.getVulkanDevice(), stagingBuffer->pSize,
            stagingBuffer->getBuffer(), vertexBuffer->getBuffer(), 0, 0);

        vulkanAPI.onNewFrame(0);

        glfwPollEvents();

        x += 0.001f;
    }
}

void Application::onFramebufferResize(GLFWwindow* window, int width, int height) {
    Application* app = (Application*) glfwGetWindowUserPointer(window);
    app->vulkanAPI.setFramebufferResized(true);
}