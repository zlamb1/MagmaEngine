#include "application.h"

struct MyVertex {
    glm::vec2 pos;
    glm::vec3 color;
};

struct UBO {
    glm::mat4 model{ 1.0f };
    glm::mat4 view{ 1.0f };
    glm::mat4 proj{ 1.0f };
};

std::vector<MyVertex> vertexData{
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f }},
    {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}
};

std::vector<uint16_t> index_data{ 0, 1, 2, 2, 3, 0 };

const char* vertexShaderCode = R"(#version 450
    layout(location = 0) in vec2 vertexPos;
    layout(location = 1) in vec3 vertexColor;
    layout(location = 0) out vec3 fragColor;

    layout(binding = 0) uniform UniformBufferObject {
        mat4 model;
        mat4 view;
        mat4 proj;
    } ubo;

    void main() {
        gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vertexPos, 0.0, 1.0);
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
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, 
        std::chrono::seconds::period>(currentTime - startTime).count();

    UBO ubo{};

    ubo.model = glm::rotate(glm::mat4(1.0f), 
        time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.model = glm::rotate(ubo.model, time * glm::radians(90.0f), 
        glm::vec3(1.0f, 0.0f, 0.0f));

    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    auto windowSize = windowImpl.getSize();
    ubo.proj = glm::perspective(glm::radians(45.0f), 
        windowSize.first / (float)windowSize.second, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    uboMemory->setData(&ubo);
}

void Application::initWindow() {
    // init glfw
    glfwInit();

    // init window
    windowImpl.init(1000, 1000);
    windowImpl.setTitle("Vulkan");
}

void Application::initVulkan() {
    vulktrixAPI.initSetup();

    // create shaders
    auto vertexShader = vulktrixAPI.createVulkanShader(vertexShaderCode, ShadercType::VERTEX);
    vulktrixAPI.getVulkanShaders().push_back(vertexShader);
    auto fragmentShader = vulktrixAPI.createVulkanShader(fragmentShaderCode, ShadercType::FRAGMENT);
    vulktrixAPI.getVulkanShaders().push_back(fragmentShader);

    auto& shaderAttributes = vulktrixAPI.getShaderAttributes();

    shaderAttributes.createVertexBinding(0, sizeof(MyVertex), 
        VertexInputRate::VERTEX);

    shaderAttributes.createVertexAttribute(0, 0, offsetof(MyVertex, pos), 
        VulkanFormat::RG_SFLOAT32);
    shaderAttributes.createVertexAttribute(0, 1, offsetof(MyVertex, color),
        VulkanFormat::RGB_SFLOAT32);
    
    auto vertexBufferSize = sizeof(MyVertex) * vertexData.size();

    // create staging buffer/memory
    stagingBuffer = vulktrixAPI.createVulkanBuffer(vertexBufferSize, 
        VulkanBufferUsage::TRANSFER_SRC);
    stagingMemory = vulktrixAPI.createDeviceMemory(stagingBuffer,
        VulkanMemoryType::CPU_VISIBLE | VulkanMemoryType::FLUSH_WRITES);
    stagingMemory->bindBufferMemory(stagingBuffer->getBuffer(), 0);

    stagingMemory->mapMemory();
    stagingMemory->setData(vertexData.data());
    stagingMemory->unmapMemory();

    // create vertex buffer/memory
    vertexBuffer = vulktrixAPI.createVulkanBuffer(vertexBufferSize,
        VulkanBufferUsage::TRANSFER_DST | VulkanBufferUsage::VERTEX);
    vertexMemory = vulktrixAPI.createDeviceMemory(vertexBuffer, 
        VulkanMemoryType::GPU_EFFICIENT);
    vertexMemory->bindBufferMemory(vertexBuffer->getBuffer(), 0);

    BufferCopy::copyBuffer(vulktrixAPI.getVulkanDevice(), stagingBuffer->pSize,
        stagingBuffer->getBuffer(), vertexBuffer->getBuffer(), 0, 0);

    vulktrixAPI.getVulkanBuffers().push_back(vertexBuffer);

    auto indexBufferSize = sizeof(uint16_t) * index_data.size();

    stagingMemory->mapMemory();
    stagingMemory->setData(index_data.data());
    stagingMemory->unmapMemory();

    // create index buffer/memory
    indexBuffer = vulktrixAPI.createVulkanBuffer(indexBufferSize,
        VulkanBufferUsage::TRANSFER_DST | VulkanBufferUsage::INDEX);
    indexMemory = vulktrixAPI.createDeviceMemory(indexBuffer, VulkanMemoryType::GPU_EFFICIENT);
    indexMemory->bindBufferMemory(indexBuffer->getBuffer(), 0);

    BufferCopy::copyBuffer(vulktrixAPI.getVulkanDevice(), indexBuffer->pSize,
        stagingBuffer->getBuffer(), indexBuffer->getBuffer(), 0, 0);

    // create ubo buffer/memory
    uboBuffer = vulktrixAPI.createVulkanBuffer(sizeof(UBO),
        VulkanBufferUsage::UNIFORM);
    uboMemory = vulktrixAPI.createDeviceMemory(uboBuffer,
        VulkanMemoryType::CPU_VISIBLE | VulkanMemoryType::FLUSH_WRITES);
    uboMemory->bindBufferMemory(uboBuffer->getBuffer(), 0);
    uboMemory->mapMemory();

    // create VulkanDescriptor
    shaderAttributes.createDescriptor();

    // create VulkanDescriptorSetLayout
    shaderAttributes.createDescriptorSetLayout();

    // create VulkanDescriptorSet
    shaderAttributes.createDescriptorSet(uboBuffer->getBuffer(), 1, sizeof(UBO));

    // upload it to the drawer
    vulktrixAPI.getVulkanDrawer()->pDescriptorSets = shaderAttributes.getDescriptorSets();

    // set draw information
    vulktrixAPI.getVulkanDrawer()->pIndexBuffer = indexBuffer;
    vulktrixAPI.getVulkanDrawer()->pIndexCount = 6;
    vulktrixAPI.getVulkanDrawer()->pUseIndexing = true;

    // init render structures
    vulktrixAPI.initRender();
}

void Application::mainLoop() {
    while (!windowImpl.shouldWindowClose()) {
        x += 0.001f;
        vertexData[0].color.x = (sin(x) + 1.0f) / 2.0f;
        vertexData[0].color.y = (sin(x) + 1.0f) / 2.0f;
        vertexData[2].color.x = (cos(x) + 1.0f) / 2.0f;
        vertexData[2].color.y = (cos(x) + 1.0f) / 2.0f;

        stagingMemory->mapMemory();
        stagingMemory->setData(vertexData.data());
        stagingMemory->unmapMemory();

        BufferCopy::copyBuffer(vulktrixAPI.getVulkanDevice(), stagingBuffer->pSize,
            stagingBuffer->getBuffer(), vertexBuffer->getBuffer(), 0, 0);

        updateUniformBuffer();

        vulktrixAPI.onNewFrame(0);

        glfwPollEvents();

        frames++;

        double currentTime = glfwGetTime();
        if (currentTime - lastTime > 1.0) {
            lastTime = currentTime;
            std::string title = "FPS: " + std::to_string(frames);
            windowImpl.setTitle(title);
            frames = 0;
        }
    }
}