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

		virtual uint32_t getMaxVertices() = 0;
		virtual void createVertices() = 0;
		virtual void createUVs() = 0;

	protected:
		Sphere& m_Sphere; 

	};

	struct UVSphereGenStrategy : SphereGenStrategy {

		UVSphereGenStrategy(Sphere& sphere);

		uint32_t getMaxVertices() override;
		void createVertices() override;
		void createUVs() override;

	};

	struct IcoSphereGenStrategy : SphereGenStrategy {

		IcoSphereGenStrategy(Sphere& sphere); 

		uint32_t getMaxVertices() override;
		void createVertices() override;
		void createUVs() override;

	};

	struct QuadSphereGenStrategy : SphereGenStrategy {

		QuadSphereGenStrategy(Sphere& sphere); 

		uint32_t getMaxVertices() override;
		void createVertices() override;
		void createUVs() override;

	};

	class Sphere : public Mesh {

	public:

		Sphere(uint32_t maxResolution, 
			SphereGenerationStrategy generationStrategy = SphereGenerationStrategy::QUAD_SPHERE);
		virtual ~Sphere() = default; 

		[[nodiscard]] std::shared_ptr<Buffer> getBuffer() const;
		[[nodiscard]] uint32_t getMaxResolution() const;
		[[nodiscard]] uint32_t getResolution() const;
		[[nodiscard]] uint32_t getVertexCount() const;
		[[nodiscard]] bool doesGenerateIndices() const;
		[[nodiscard]] bool doesGenerateUVs() const;

		void setGenerationStrategy(SphereGenerationStrategy generationStrategy);
		void setResolution(uint32_t resolution); 
		void setVertexCount(uint32_t vertexCount);

		virtual void create(RenderCore& renderCore);
		void createNormals();

	private:
		uint32_t m_MaxResolution = 0, m_Resolution = 0, m_VertexCount = 0;
		bool m_GenerateIndices = false, m_GenerateNormals = true, m_GenerateUVs = true;
		std::unique_ptr<SphereGenStrategy> m_GenStrategy; 

		std::shared_ptr<Buffer> m_Buffer = nullptr;

	};


	// much of the code for the UVSphere implementation is from
	// https://gist.github.com/Pikachuxxxx/5c4c490a7d7679824e0e18af42918efc
	// everything else is written by me

	//namespace IcoSphere {

	const float t = (1.0f + sqrt(5.0f)) / 2.0f;
	const std::vector<glm::vec3> m_IcosahedronPositions{
		{t, 1.0f, 0}, {-t, 1.0f, 0.0f}, { t, -1.0f, 0.0f },
		{-t, -1.0f, 0.0f}, {1.0f, 0.0f, t}, {1.0f, 0.0f, -t},
		{-1.0f, 0.0f, t}, {-1.0f, 0.0f, -t}, {0.0f, t, 1.0f},
		{0.0f, -t, 1.0f}, {0.0f, t, -1.0f}, {0.0f, -t, -1.0f}
	};
	const std::vector<uint16_t> m_IcosahedronIndices{
		0, 8, 4, 0, 5, 10, 2, 4, 9, 2, 11, 5, 1, 6, 8, 1, 10, 7, 3, 9, 6, 3, 7, 11, 0, 10, 8, 1, 8, 10,
		2, 9, 11, 3, 9, 11, 4, 2, 0, 5, 0, 2, 6, 1, 3, 7, 3, 1, 8, 6, 4, 9, 4, 6, 10, 5, 7, 11, 7, 5
	};

	//	static SphereData createSphere(int resolution) {
	//		SphereData sphereData{};

	//		sphereData.m_Indices = m_IcosahedronIndices;

	//		std::vector<glm::vec3> inVerts = m_IcosahedronPositions;
	//		std::vector<uint16_t> inIndices = sphereData.m_Indices;

	//		if (resolution == 0) {
	//			for (auto& position : inVerts) {
	//				position = glm::normalize(position);
	//			}
	//		}

	//		for (int j = 0; j < resolution; j++) {
	//			std::vector<glm::vec3> nVerts{};
	//			std::vector<uint16_t> nIndices{};
	//			for (int i = 0; i < inIndices.size(); i += 3) {
	//				glm::vec3 a = inVerts[inIndices[i]];
	//				auto aIndex = SphereUtility::getIndexAndAdd(nVerts, a);

	//				glm::vec3 b = inVerts[inIndices[i + 1]];
	//				auto bIndex = SphereUtility::getIndexAndAdd(nVerts, b);

	//				glm::vec3 c = inVerts[inIndices[i + 2]];
	//				auto cIndex = SphereUtility::getIndexAndAdd(nVerts, c);

	//				glm::vec3 d = b + ((a - b) / 2.0f);
	//				auto dIndex = SphereUtility::getIndexAndAdd(nVerts, d);

	//				glm::vec3 e = c + ((b - c) / 2.0f);
	//				auto eIndex = SphereUtility::getIndexAndAdd(nVerts, e);

	//				glm::vec3 f = c + ((a - c) / 2.0f);
	//				auto fIndex = SphereUtility::getIndexAndAdd(nVerts, f);
	//				
	//				std::vector<uint16_t> _indices{
	//					aIndex, dIndex, fIndex,
	//					dIndex, bIndex, eIndex,
	//					eIndex, fIndex, dIndex,
	//					fIndex, eIndex, cIndex
	//				};

	//				nIndices.insert(nIndices.begin(), _indices.begin(), _indices.end());
	//			}
	//			inVerts = nVerts;
	//			inIndices = nIndices;
	//		}

	//		for (auto& pos : inVerts) {
	//			sphereData.m_Vertices.emplace_back(Vertex{ glm::normalize(pos) });
	//		}

	//		sphereData.m_Indices = inIndices;

	//		for (auto& vertex : sphereData.m_Vertices) {
	//			// a vertex on a sphere is its own normal
	//			vertex.m_Normal = vertex.m_Position;
	//			vertex.m_UV = { vertex.m_Normal.x / 2.0f + 0.5f, vertex.m_Normal.y / 2.0f + 0.5f };
	//		}

	//		return sphereData;
	//	}

	//}

}