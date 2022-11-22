#include "application.h"

struct MyVertexState : _VkVertexState {
    VkVertexInputBindingDescription getBindingDescription() override {
        pBindingDescription.binding = 0;
        pBindingDescription.stride = sizeof(float) * 5;
        return pBindingDescription.getBindingDescription();
    }
    
    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() override {
        pAttributeDescriptions.resize(2);
        pAttributeDescriptions[0].binding = 0;
        pAttributeDescriptions[0].location = 0;
        pAttributeDescriptions[0].format = _VkFormat::R32G32_SFLOAT;
        pAttributeDescriptions[0].offset = 0;
        pAttributeDescriptions[1].binding = 0;
        pAttributeDescriptions[1].location = 1;
        pAttributeDescriptions[1].format = _VkFormat::R32G32B32_SFLOAT;
        pAttributeDescriptions[1].offset = sizeof(float) * 2;
        return _VkVertexState::getAttributeDescriptions();
    }
};

const std::vector<float> vertex_data = {
    0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
};

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

// Public Implementation

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

    return EXIT_SUCCESS;
}

// Private

Application::~Application() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::initWindow() {
    // init
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    // set callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, onFramebufferResize);

    // set window pos
    const GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwSetWindowPos(window, mode->width - WIDTH, 30);
}

void Application::initVulkan() {
    vulkanAPI = std::make_unique<VulkanAPI>(*window);
    vulkanAPI->initSetup();

    auto vertexShader = vulkanAPI->createShaderHandle(vertexShaderCode, _ShaderType::VERTEX);
    vulkanAPI->addShaderHandle(vertexShader);
    auto fragmentShader = vulkanAPI->createShaderHandle(fragmentShaderCode, _ShaderType::FRAGMENT);
    vulkanAPI->addShaderHandle(fragmentShader);
    
    auto myVertex = MyVertexState{};
    vulkanAPI->addVertexInputState(myVertex);

    auto buffer = vulkanAPI->createBufferHandle(sizeof(float) * vertex_data.size());
    buffer->setData(vertex_data.data());
    vulkanAPI->addBufferHandle(buffer);

    vulkanAPI->initRender();
}

void Application::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        vulkanAPI->onNewFrame(3);

        glfwPollEvents();
    }
}

void Application::onFramebufferResize(GLFWwindow* window, int width, int height) {
    Application* app = (Application*) glfwGetWindowUserPointer(window);
    app->vulkanAPI->setFramebufferResized(true);
}