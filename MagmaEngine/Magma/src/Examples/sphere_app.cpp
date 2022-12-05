#include "sphere_app.h"

namespace Magma {

    struct UBO {
        glm::mat4 model{ 1.0f };
        glm::mat4 view { 1.0f };
        glm::mat4 proj { 1.0f };
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

    SphereApp::~SphereApp() {
        glfwTerminate();
    }

    void SphereApp::onNewFrame() {
        vertexBuffer->setData(sphereData.verts.data(),
            sphereData.verts.size() * sizeof(SphereVertex));
        indexBuffer->setData(sphereData.indices.data(),
            sphereData.indices.size() * sizeof(uint16_t));

        renderCore->getRenderer().setIndexCount(sphereData.indices.size());

        sphereStep.onNewFrame();
        if (sphereStep.getElapsed() > 0.4) {
            updateSphereData();
            sphereStep.reset();
        }

        cameraStep.onNewFrame();

        if (input->isKeyPressed(KeyButton::C) && cameraStep.getElapsed() >= 1.0f) {
            cameraIndex++;
            if (cameraIndex % 2 == 0)
                camera.reset(new ThirdPersonImpl(*input));
            else
                camera.reset(new FirstPersonImpl(*input));
            cameraStep.reset();
        }

        camera->onUpdate(frameStep);

        updateUniformBuffer();

        renderCore->onNewFrame();

        glfwPollEvents();

        frameStep.onNewFrame();
        if (frameStep.getElapsed() > 0.2) {
            std::string title = "FPS: " + std::to_string(frameStep.getFps());
            windowImpl->setTitle(title);
            frameStep.reset();
        }
    }

    void SphereApp::initWindow() {
        // init glfw
        glfwInit();

        // init window
        windowImpl = std::make_shared<GLFWImpl>();
        windowImpl->init(500, 500);
        windowImpl->setTitle("Vulkan");
        
        // init input
        input = std::make_shared<WindowInput>(*windowImpl);

        // init camera
        camera = std::make_shared<ThirdPersonImpl>(*input);
    }

    void SphereApp::initVulkan() {
        Application::initVulkan(); 

        renderCore->init();

        // create shaders
        auto vertexShader = renderCore->createShader(vertexShaderCode, ShadercType::VERTEX);
        renderCore->getShaders().push_back(vertexShader);
        auto fragmentShader = renderCore->createShader(fragmentShaderCode, ShadercType::FRAGMENT);
        renderCore->getShaders().push_back(fragmentShader);

        auto& shaderAttributes = renderCore->getShaderAttributes();

        shaderAttributes.createVertexBinding(0, sizeof(SphereVertex), VertexInputRate::VERTEX);
        shaderAttributes.createVertexAttribute(0, 0, offsetof(SphereVertex, pos),
            DataFormat::RGB_SFLOAT32);
        shaderAttributes.createVertexAttribute(0, 1, offsetof(SphereVertex, color),
            DataFormat::RGB_SFLOAT32);

        resolution = MIN_RESOLUTIONS[sphereMode];
        updateSphereData();

        // create vertex buffer
        auto vertexBufferSize = sizeof(SphereVertex) * 5000;
        vertexBuffer = renderCore->createBuffer(vertexBufferSize, BufferUsage::VERTEX);
        vertexBuffer->map();
        vertexBuffer->setData(sphereData.verts.data(), 
            sizeof(SphereVertex) * sphereData.verts.size());

        renderCore->addBuffer(vertexBuffer);

        // create index buffer
        auto indexBufferSize = sizeof(uint16_t) * 30000;
        indexBuffer = renderCore->createBuffer(indexBufferSize, BufferUsage::INDEX);
        indexBuffer->map();
        indexBuffer->setData(sphereData.indices.data(), 
            sizeof(uint16_t) * sphereData.indices.size());

        // create uniform buffer
        uniformBuffer = renderCore->createBuffer(sizeof(UBO), BufferUsage::UNIFORM);
        uniformBuffer->map();

        // create descriptor
        shaderAttributes.createDescriptor();

        // create descriptor set layout
        shaderAttributes.createDescriptorSetLayout();

        // create descriptor set
        shaderAttributes.createDescriptorSet(*uniformBuffer, 1, sizeof(UBO));

        // set renderer fields
        renderCore->getRenderer().setIndexBuffer(indexBuffer);
        renderCore->getRenderer().setUseIndexing(true);

        // init render structures
        renderCore->initRender();
    }

    void SphereApp::updateUniformBuffer() {
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

    glm::vec3 SphereApp::getRandomColor() {
        return glm::vec3{
            (float)rand() / (float)RAND_MAX,
            (float)rand() / (float)RAND_MAX,
            (float)rand() / (float)RAND_MAX
        };
    }

    void SphereApp::updateSphereData() {
        if (resolution >= MAX_RESOLUTIONS[sphereMode]) {
            sphereMode = (sphereMode + 1) % 3;
            resolution = MIN_RESOLUTIONS[sphereMode];
        }

        switch (sphereMode) {
        case 0:
            sphereData = UVSphere::createSphere(resolution);
            break;
        case 1:
            sphereData = IcoSphere::createSphere(resolution);
            break;
        case 2:
            sphereData = QuadSphere::createSphere(resolution);
            break;
        }

        for (int i = 0; i < sphereData.verts.size(); i++) {
            sphereData.verts[i].color = getRandomColor();
        }

        resolution++;
    }

}