#pragma once

#include <memory>

#include "Magma/Buffer/buffer.h"
#include "Magma/Render/renderer.h"
#include "shader_attributes.h"

namespace Magma {

	class RenderCore {

	public:
		virtual void init() = 0; 
		virtual void initRender() = 0;
		virtual void onNewFrame() = 0;

		virtual Renderer& getRenderer() const = 0;
		virtual ShaderAttributes& getShaderAttributes() const = 0; 

		virtual std::shared_ptr<Buffer> createBuffer(int64_t size) = 0;
		virtual std::shared_ptr<Buffer> createBuffer(int64_t size, BufferUsage usage) = 0;
		virtual void setDepthBuffering(bool enabled) = 0; 

		virtual void addBuffer(std::shared_ptr<Buffer> buffer) = 0;

	};

}