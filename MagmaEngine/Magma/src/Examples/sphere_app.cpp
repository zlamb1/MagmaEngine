#include "sphere_app.h"

#include "Magma/Image/STB/stb_image.h"

#include "Magma/Entity/Camera/tp_camera.h"
#include "Magma/Image/STB/stb_loader.h"
#include "Magma/Window/glfw_impl.h"

namespace Magma {

	struct UBO {
		glm::mat4 model{1.0f};
		glm::mat4 view{1.0f};
		glm::mat4 proj{1.0f};
	};

	const char* vertexShaderCode = R"(#version 450
        layout(binding = 0) uniform UniformBufferObject {
            mat4 model;
            mat4 view;
            mat4 proj;
        } ubo;

        layout(location = 0) in vec3 vertexPos;
        layout(location = 1) in vec3 vertexColor;
        layout(location = 2) in vec3 vertexNormal; 
        layout(location = 3) in vec2 vertexUV;

        layout(location = 0) out vec3 fragColor;
        layout(location = 1) out vec3 fragNormal;
        layout(location = 2) out vec2 fragUV;

        void main() {
            gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vertexPos, 1.0);
            fragColor = vertexColor;
            fragNormal = vertexNormal;
            fragUV = vertexUV;
        })";

	const char* fragmentShaderCode = R"(#version 450
        layout(location = 0) in vec3 fragColor;
        layout(location = 1) in vec3 fragNormal;
        layout(location = 2) in vec2 fragUV; 

        layout(location = 0) out vec4 outColor;
        
        layout(binding = 1) uniform Time {
            float time; 
        } iTime;
		
		layout(binding = 2) uniform sampler2D textureSampler;

        void main() {
            float ambient = 0.1;
            vec3 lightDir = vec3(cos(iTime.time), -1, sin(iTime.time));
            float lighting = max(ambient, dot(fragNormal, -normalize(lightDir))); 
            outColor = texture(textureSampler, fragUV) * lighting;
        })";

	SphereApp::~SphereApp() {
		glfwTerminate();
	}

	void SphereApp::onNewFrame() {
		m_VertexBuffer->setData(m_SphereData.verts.data(),
			m_SphereData.verts.size() * sizeof(SphereVertex));

		m_TimeStep.onNewFrame();
		auto time = static_cast<float>(m_TimeStep.getElapsed());
		m_TimeBuffer->setData(&time, sizeof(float));

		m_RenderCore->getRenderer().setVertexCount(static_cast<uint32_t>(m_SphereData.verts.size()));

		m_SphereStep.onNewFrame();
		if (m_SphereStep.getElapsed() > 0.4) {
			updateSphereData();
			m_SphereStep.reset();
		}

		m_CameraStep.onNewFrame();
		if (m_Input->isKeyPressed(KeyButton::C) && m_CameraStep.getElapsed() >= 1.0) {
			m_CameraIndex++;
			if (m_CameraIndex % 2 == 0)
				m_Camera.reset(new ThirdPersonImpl(*m_Input));
			else
				m_Camera.reset(new FirstPersonImpl(*m_Input));
			m_CameraStep.reset();
		}

		m_Camera->onUpdate(m_FrameStep);

		updateUniformBuffer();

		m_RenderCore->onNewFrame();

		glfwPollEvents();

		m_FrameStep.onNewFrame();
		if (m_FrameStep.getElapsed() > 0.2) {
			const std::string title = "FPS: " + std::to_string(m_FrameStep.getFps());
			m_WindowImpl->setTitle(title);
			m_FrameStep.reset();
		}
	}

	void SphereApp::initWindow() {
		// init GLFW
		glfwInit();

		// init window
		m_WindowImpl = std::make_shared<GLFWImpl>();
		m_WindowImpl->init(500, 500);
		m_WindowImpl->setTitle("Vulkan");

		// init m_Input
		m_Input = std::make_shared<WindowInput>(*m_WindowImpl);

		// init camera
		m_Camera = std::make_shared<ThirdPersonImpl>(*m_Input);
	}

	void SphereApp::initVulkan() {
		Application::initVulkan();

		m_RenderCore->init();
		m_RenderCore->setDepthBuffering(true);

		// create shaders
		const auto vertexShader = m_RenderCore->createShader(vertexShaderCode, VERTEX);
		m_RenderCore->getShaders().push_back(vertexShader);
		const auto fragmentShader = m_RenderCore->createShader(fragmentShaderCode, FRAGMENT);
		m_RenderCore->getShaders().push_back(fragmentShader);

		auto& shaderAttributes = m_RenderCore->getShaderAttributes();
		shaderAttributes.createVertexBinding(0, sizeof(SphereVertex), VertexInputRate::VERTEX);
		shaderAttributes.createVertexAttribute(0, 0, offsetof(SphereVertex, pos),
		                                       DataFormat::R32G32B32_SFLOAT);
		shaderAttributes.createVertexAttribute(0, 1, offsetof(SphereVertex, color),
		                                       DataFormat::R32G32B32_SFLOAT);
		shaderAttributes.createVertexAttribute(0, 2, offsetof(SphereVertex, normal),
		                                       DataFormat::R32G32B32_SFLOAT);
		shaderAttributes.createVertexAttribute(0, 3, offsetof(SphereVertex, uv),
		                                       DataFormat::R32G32_SFLOAT);

		m_Resolution = MIN_RESOLUTIONS[m_SphereMode];
		updateSphereData();

		// create vertex buffer
		constexpr auto vertexBufferSize = sizeof(SphereVertex) * 100000;
		m_VertexBuffer = m_RenderCore->createBuffer(vertexBufferSize, BufferUsage::VERTEX);
		m_VertexBuffer->map();
		m_VertexBuffer->setData(m_SphereData.verts.data(),
		                      sizeof(SphereVertex) * m_SphereData.verts.size());

		m_RenderCore->addBuffer(m_VertexBuffer);

		// create ubo buffer
		m_UboBuffer = m_RenderCore->createBuffer(sizeof(UBO), BufferUsage::UNIFORM);
		m_UboBuffer->map();

		// create time buffer
		m_TimeBuffer = m_RenderCore->createBuffer(sizeof(float), BufferUsage::UNIFORM);
		m_TimeBuffer->map();

		// create texture resources
		createTexture();

		// create descriptor
		shaderAttributes.createUniformDescriptor(m_UboBuffer, 0, sizeof(UBO),
		                                         1, VulkanShaderType::VERTEX);
		shaderAttributes.createUniformDescriptor(m_TimeBuffer, 1, sizeof(float),
		                                         1, VulkanShaderType::FRAGMENT);
		shaderAttributes.createImageDescriptor(m_TextureImageView, m_TextureSampler, 2, 1,
		                                       VulkanShaderType::FRAGMENT);

		// create descriptor set layout
		shaderAttributes.createDescriptorSetLayout();

		// create descriptor set
		shaderAttributes.createDescriptorSet(1);

		// set renderer fields
		// m_RenderCore->getRenderer().setIndexBuffer(indexBuffer);
		// m_RenderCore->getRenderer().setUseIndexing(true);

		// init render structures
		m_RenderCore->initRender();
	}

	void SphereApp::updateUniformBuffer() const {
		static auto start = Timestep();
		start.onNewFrame();
		UBO ubo{
			glm::mat4{1.0f},
			m_Camera->getViewMat4f(),
			m_Camera->getPerspectiveMat4f()
		};
		m_UboBuffer->setData(&ubo, sizeof(ubo));
	}

	void SphereApp::updateSphereData() {
		if (m_Resolution >= MAX_RESOLUTIONS[m_SphereMode]) {
			m_SphereMode = (m_SphereMode + 1) % 3;
			m_Resolution = MIN_RESOLUTIONS[m_SphereMode];
		}

		// manually set values
		m_SphereMode = 2;
		m_Resolution = 5;

		switch (m_SphereMode) {
			case 0:
				m_SphereData = UVSphere::createSphere(m_Resolution);
				break;
			case 1:
				m_SphereData = IcoSphere::createSphere(m_Resolution);
				break;
			case 2:
				m_SphereData = QuadSphere::createSphere(m_Resolution, false);
				break;
			default:
				break;
		}

		m_Resolution++;
	}

	void SphereApp::createTexture() {
		StbImageLoader loader{};
		const auto imageData = loader.loadImage("texture1.jpg");

		const int64_t imageSize = static_cast<int64_t>(imageData->m_Width) * imageData->m_Height * 4;

		if (!imageData->m_Pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		m_TextureBuffer = m_RenderCore->createBuffer(imageSize, BufferUsage::TRANSFER_SRC);
		m_TextureBuffer->map();
		m_TextureBuffer->setData(imageData->m_Pixels, imageSize);
		m_TextureBuffer->unmap();

		m_TextureImage = m_RenderCore->createTexture(imageData->m_Width, imageData->m_Height);

		TransitionImage transitionImage{m_RenderCore->getDevice()};
		transitionImage.m_Image = m_TextureImage;
		transitionImage.m_OldLayout = ImageLayout::UNDEFINED;
		transitionImage.m_NewLayout = ImageLayout::TRANSFER_DST_OPTIMAL;
		transitionImage.init();

		CopyBufferToImage copyBufferToImage{m_RenderCore->getDevice()};
		copyBufferToImage.pBuffer = m_TextureBuffer;
		copyBufferToImage.pImage = m_TextureImage;
		copyBufferToImage.init();

		transitionImage.m_OldLayout = ImageLayout::TRANSFER_DST_OPTIMAL;
		transitionImage.m_NewLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
		transitionImage.init();

		m_TextureImageView = m_RenderCore->createTextureImageView(m_TextureImage);
		m_TextureSampler = m_RenderCore->createSampler();
	}
}
