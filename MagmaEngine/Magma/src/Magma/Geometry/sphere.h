#pragma once

#include "cube.h"
#include "mesh.h"

#include <glm/gtc/constants.hpp>

#include "sphere.h"

#include "Magma/Render/render_core.h"

namespace Magma {

	namespace SphereUtility {

		static uint16_t getIndex(std::vector<glm::vec3>& vertices, const glm::vec3& position) {
			const auto iter = std::ranges::find(vertices.begin(),
				vertices.end(), position);
			if (iter == vertices.end()) {
				return static_cast<uint16_t>(vertices.size());
			}

			return static_cast<uint16_t>(std::distance(vertices.begin(), iter));
		}

		static uint16_t getIndexAndAdd(std::vector<glm::vec3>& vertices, glm::vec3 position) {
			const auto index = getIndex(vertices, position);
			if (index == vertices.size()) {
				vertices.push_back(position);
			}
			return index;
		}

		static void duplicateVertices(std::vector<Vertex>& vertices) {
			for (int i = static_cast<int>(vertices.size()) - 1; i >= 0; i--) {
				auto& [pos, normal, UV] = vertices[i];
				if (UV.x <= 0.05f) {
					const int triangleIndex = 3 * (i / 3);
					for (int j = 2; j >= 0; j--) {
						const auto& [oPos, oNormal, oUV] = vertices[triangleIndex + j];
						if (abs(UV.x - oUV.x) >= 0.5f) {
							// set U coordinate to 0.0f to complete the wrap
							UV.x = 1.0f;
							break;
						}
					}
				}
			}
		}
	}

	class Sphere;

	enum class SphereGenerationStrategy : int {
		UV_SPHERE,
		ICO_SPHERE,
		QUAD_SPHERE
	};

	struct SphereGenStrategy {

		SphereGenStrategy(Sphere& sphere); 
		virtual ~SphereGenStrategy() = default; 

		virtual uint32_t getMaxVertices(int resolution) = 0;
		virtual uint32_t getMaxIndices(int resolution) = 0; 
		virtual void createVertices() = 0;

	protected:
		Sphere& m_Sphere; 

	};

	struct UVSphereGenStrategy : SphereGenStrategy {

		UVSphereGenStrategy(Sphere& sphere);

		uint32_t getMaxVertices(int resolution) override;
		uint32_t getMaxIndices(int resolution) override;
		void createVertices() override;

	};

	struct IcoSphereGenStrategy : SphereGenStrategy {

		IcoSphereGenStrategy(Sphere& sphere); 

		uint32_t getMaxVertices(int resolution) override;
		uint32_t getMaxIndices(int resolution) override;
		void createVertices() override;

	};

	struct QuadSphereGenStrategy : SphereGenStrategy {

		QuadSphereGenStrategy(Sphere& sphere); 

		uint32_t getMaxVertices(int resolution) override;
		uint32_t getMaxIndices(int resolution) override;
		void createVertices() override;

	};

	class Sphere : public Mesh {

	public:

		Sphere(uint32_t maxResolution, 
			SphereGenerationStrategy generationStrategy = SphereGenerationStrategy::QUAD_SPHERE);
		virtual ~Sphere() = default; 

		[[nodiscard]] std::shared_ptr<Buffer> getVertexBuffer() const;
		[[nodiscard]] std::shared_ptr<Buffer> getIndexBuffer() const; 
		[[nodiscard]] uint32_t getMaxResolution() const;
		[[nodiscard]] uint32_t getResolution() const;

		[[nodiscard]] bool doesGenerateIndices() const;
		[[nodiscard]] bool doesGenerateUVs() const;

		void setGenerationStrategy(SphereGenerationStrategy generationStrategy);
		void setResolution(uint32_t resolution); 

		virtual void create(RenderCore& renderCore);
		virtual void createNormals();
		virtual void createUVs(); 

	private:
		uint32_t m_MaxResolution = 0, m_Resolution = 0;
		bool m_GenerateIndices = false, m_GenerateNormals = true, m_GenerateUVs = true;
		std::unique_ptr<SphereGenStrategy> m_GenStrategy; 

		std::shared_ptr<Buffer> m_VertexBuffer = nullptr, m_IndexBuffer = nullptr;

	};

	const float t = (1.0f + sqrt(5.0f)) / 2.0f;
	const std::vector<glm::vec3> ICOSAHEDRON_POSITIONS {
		{t, 1.0f, 0}, {-t, 1.0f, 0.0f}, { t, -1.0f, 0.0f },
		{-t, -1.0f, 0.0f}, {1.0f, 0.0f, t}, {1.0f, 0.0f, -t},
		{-1.0f, 0.0f, t}, {-1.0f, 0.0f, -t}, {0.0f, t, 1.0f},
		{0.0f, -t, 1.0f}, {0.0f, t, -1.0f}, {0.0f, -t, -1.0f}
	};
	const std::vector<uint16_t> ICOSAHEDRON_INDICES {
		0, 8, 4, 0, 5, 10, 2, 4, 9, 2, 11, 5, 1, 6, 8, 1, 10, 7, 3, 9, 6, 3, 7, 11, 0, 10, 8, 1, 8, 10,
		2, 9, 11, 3, 9, 11, 4, 2, 0, 5, 0, 2, 6, 1, 3, 7, 3, 1, 8, 6, 4, 9, 4, 6, 10, 5, 7, 11, 7, 5
	};

}