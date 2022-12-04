#include "application.h"

namespace Magma {

    struct MyVertex {
        MyVertex(glm::vec3 pos) : pos{ pos }, color{ 0.0f } {}

        glm::vec3 pos;
        glm::vec3 color;
    };

    struct UBO {
        glm::mat4 model{ 1.0f };
        glm::mat4 view{ 1.0f };
        glm::mat4 proj{ 1.0f };
    };

    std::vector<MyVertex> vertexData{
        // SOUTH
        {{-0.5f, -0.5f, -0.5f}},
        {{0.5f, -0.5f, -0.5f}},
        {{0.5f, 0.5f, -0.5f}},
        {{-0.5f, 0.5f, -0.5f}},
        // NORTH
        {{0.5f, -0.5f, 0.5f}},
        {{-0.5f, -0.5f, 0.5f}},
        {{-0.5f, 0.5f, 0.5f}},
        {{0.5f, 0.5f, 0.5f}},
        // WEST
        {{-0.5f, -0.5f, 0.5f}},
        {{-0.5f, -0.5f, -0.5f}},
        {{-0.5f, 0.5f, -0.5f}},
        {{-0.5f, 0.5f, 0.5f}},
        // EAST
        {{0.5f, -0.5f, -0.5f}},
        {{0.5f, -0.5f, 0.5f}},
        {{0.5f, 0.5f, 0.5f}},
        {{0.5f, 0.5f, -0.5f}},
        // BOTTOM
        {{-0.5f, -0.5f, -0.5f}},
        {{0.5f, -0.5f, -0.5f}},
        {{0.5f, -0.5f, 0.5f}},
        {{-0.5f, -0.5f, 0.5f}},
        // TOP
        {{0.5f, 0.5f, 0.5f}},
        {{-0.5f, 0.5f, 0.5f}},
        {{-0.5f, 0.5f, -0.5f}},
        {{0.5f, 0.5f, -0.5f}}
    };

    std::vector<uint16_t> indexData{
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    const char* vertexShaderCode = R"(#version 450
    layout(binding = 0) uniform UniformBufferObject {
        mat4 model;
        mat4 view;
        mat4 proj;
    } ubo;

    layout(location = 0) in vec3 vertexPos;
    layout(location = 1) in vec3 vertexColor;

    layout(location = 0) out vec3 fragColor;

    void main() {
        gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vertexPos, 1.0);
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

        return exit_status;
    }

    // Private

    Application::~Application() {
        glfwTerminate();
    }

    void Application::updateUniformBuffer() {
        static auto start = Timestep();
        start.onNewFrame();

        UBO ubo{};
        ubo.model = glm::rotate(glm::mat4{ 1.0f },
            (float)start.getElapsed() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.model = glm::mat4(1.0f);
        ubo.view = camera->getViewMat4f();
        ubo.proj = camera->getPerspectiveMat4f();

        uniformBuffer->setData(&ubo, sizeof(ubo));
    }

    void Application::initWindow() {
        // init glfw
        glfwInit();

        // init window
        windowImpl.init(500, 500);
        windowImpl.setTitle("Vulkan");
        windowImpl.setPosition(2000, 300);

        camera = std::make_unique<ThirdPersonImpl>(input);
    }

    void Application::initVulkan() {
        vulkanAPI.initSetup();

        // create shaders
        auto vertexShader = vulkanAPI.createVulkanShader(vertexShaderCode, ShadercType::VERTEX);
        vulkanAPI.getVulkanShaders().push_back(vertexShader);
        auto fragmentShader = vulkanAPI.createVulkanShader(fragmentShaderCode, ShadercType::FRAGMENT);
        vulkanAPI.getVulkanShaders().push_back(fragmentShader);

        auto& shaderAttributes = vulkanAPI.getShaderAttributes();

        shaderAttributes.createVertexBinding(0, sizeof(MyVertex),
            VertexInputRate::VERTEX);

        shaderAttributes.createVertexAttribute(0, 0, offsetof(MyVertex, pos),
            VulkanFormat::RGB_SFLOAT32);
        shaderAttributes.createVertexAttribute(0, 1, offsetof(MyVertex, color),
            VulkanFormat::RGB_SFLOAT32);

        // create vertex buffer/memory
        auto vertexBufferSize = sizeof(MyVertex) * vertexData.size();
        vertexBuffer = vulkanAPI.createBuffer(vertexBufferSize,
            BufferUsage::VERTEX);
        vertexBuffer->map();
        vertexBuffer->setData(vertexData.data(), vertexData.size() * sizeof(MyVertex));

        vulkanAPI.getBuffers().push_back(vertexBuffer);

        // create index buffer/memory
        auto indexBufferSize = sizeof(uint16_t) * indexData.size();
        indexBuffer = vulkanAPI.createBuffer(indexBufferSize,
            BufferUsage::INDEX);
        indexBuffer->map();
        indexBuffer->setData(indexData.data(), indexBufferSize);

        // create ubo buffer/memory
        uniformBuffer = vulkanAPI.createBuffer(sizeof(UBO), BufferUsage::UNIFORM);
        uniformBuffer->map();

        // create VulkanDescriptor
        shaderAttributes.createDescriptor();

        // create VulkanDescriptorSetLayout
        shaderAttributes.createDescriptorSetLayout();

        // create VulkanDescriptorSet
        shaderAttributes.createDescriptorSet(uniformBuffer->getBuffer(), 1, sizeof(UBO));

        // upload it to the drawer
        vulkanAPI.getVulkanDrawer()->pDescriptorSets = shaderAttributes.getDescriptorSets();

        // set draw information
        vulkanAPI.getVulkanDrawer()->pIndexBuffer = indexBuffer;
        vulkanAPI.getVulkanDrawer()->pIndexCount = indexData.size();
        vulkanAPI.getVulkanDrawer()->pUseIndexing = true;

        // init render structures
        vulkanAPI.initRender();
    }

    glm::vec3 getColor(glm::vec2 uv, float iTime) {
        return 0.5f + 0.5f * cos(iTime + glm::vec3(uv.x, uv.y, uv.x) + glm::vec3(0, 2, 4));
    }

    void Application::mainLoop() {
        Timestep lastCameraChange{};
        while (!windowImpl.shouldWindowClose()) {
            x += 0.001f;
            for (int i = 0; i < 6; i++) {
                for (int j = 0; j < 4; j++) {
                    vertexData[j + i * 4].color = getColor(glm::vec2{ 0.0f }, x);
                    vertexData[j + i * 4].color = getColor(glm::vec2{ 1.0f, 0.0f }, x);
                    vertexData[j + i * 4].color = getColor(glm::vec2{ 1.0f, 1.0f }, x);
                    vertexData[j + i * 4].color = getColor(glm::vec2{ 1.0f, 0.0f }, x);
                }
            }
            vertexBuffer->setData(vertexData.data(), vertexData.size() * sizeof(MyVertex));

            lastCameraChange.onNewFrame();

            if (input.isKeyPressed(KeyButton::C) && lastCameraChange.getElapsed() >= 1.0f) {
                cameraIndex++;
                if (cameraIndex % 2 == 0) {
                    camera.reset(new ThirdPersonImpl(input));
                }
                else {
                    camera.reset(new FirstPersonImpl(input));
                }
                lastCameraChange.reset();
            }

            camera->onUpdate(step);

            updateUniformBuffer();

            vulkanAPI.onNewFrame(0);

            glfwPollEvents();

            step.onNewFrame();

            if (step.getElapsed() > 0.5) {
                std::string title = "FPS: " + std::to_string(step.getFps());
                windowImpl.setTitle(title);
                step.reset();
            }
        }
    }

}