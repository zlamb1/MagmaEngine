#include "application.h"

struct MyVertexState : VulkanVertexState {
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
    0.0f, -0.5f, 1.0f, 0.0f, 0.0f,
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
    vulkanAPI.initSetup(window);

    auto vertexShader = vulkanAPI.createShaderHandle(vertexShaderCode, VulkanShaderType::VERTEX);
    vulkanAPI.getShaderHandles().push_back(vertexShader);
    auto fragmentShader = vulkanAPI.createShaderHandle(fragmentShaderCode, VulkanShaderType::FRAGMENT);
    vulkanAPI.getShaderHandles().push_back(fragmentShader);
    
    auto myVertex = MyVertexState{};
    vulkanAPI.addVertexInputState(myVertex);

    vulkanBufferHandle = vulkanAPI.createBufferHandle(sizeof(float) * vertex_data.size());
    vulkanAPI.getBufferHandles().push_back(vulkanBufferHandle);

    vulkanAPI.initRender();
}

namespace MapUtility {
    static float map(float in, float min, float max, float dMin, float dMax) {
        double slope = 1.0 * (dMax - dMin) / (max - min);
        return dMin + slope * (in - min);
    }
}

void Application::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        vertex_data[2] = MapUtility::map(sin(x), -1.0f, 1.0f, 0.0f, 1.0f);
        vertex_data[8] = MapUtility::map(sin(x + 1.0f), -1.0f, 1.0f, 0.0f, 1.0f);
        vertex_data[14] = MapUtility::map(sin(x + 2.0f), -1.0f, 1.0f, 0.0f, 1.0f);
        vulkanBufferHandle->setData(vertex_data.data());

        vulkanAPI.onNewFrame(3);

        glfwPollEvents();

        x += 0.001f;
    }
}

void Application::onFramebufferResize(GLFWwindow* window, int width, int height) {
    Application* app = (Application*) glfwGetWindowUserPointer(window);
    app->vulkanAPI.setFramebufferResized(true);
}