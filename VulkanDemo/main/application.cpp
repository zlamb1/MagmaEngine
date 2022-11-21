#include "application.h"

const char* vertexShaderCode = R"(#version 450
    layout(location = 0) out vec3 fragColor;
    vec2 positions[3] = vec2[](
        vec2(0.0, -0.5),
        vec2(0.5, 0.5),
        vec2(-0.5, 0.5)
    );
    vec3 colors[3] = vec3[](
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 0.0, 1.0)
    );
    void main() {
        gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
        fragColor = colors[gl_VertexIndex];
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

    vulkanAPI->initRender();
}

void Application::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        vulkanAPI->onNewFrame();

        glfwPollEvents();
    }
}

void Application::onFramebufferResize(GLFWwindow* window, int width, int height) {
    Application* app = (Application*) glfwGetWindowUserPointer(window);
    app->vulkanAPI->setFramebufferResized(true);
}