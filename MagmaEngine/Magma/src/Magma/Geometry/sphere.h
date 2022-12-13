#pragma once

#include "cube.h"

namespace Magma {

	struct SphereVertex {
		SphereVertex() : m_Position{}, m_Normal{}, m_UV{} {}
		SphereVertex(glm::vec3 position) : m_Position{ position }, m_Normal{}, m_UV{} {}

		glm::vec3 m_Position, m_Normal;
		glm::vec2 m_UV;
	};

	struct SphereData {
		std::vector<SphereVertex> m_Vertices{};
		std::vector<uint16_t> m_Indices{};
	};

	namespace SphereUtility {

		static uint16_t getIndex(std::vector<glm::vec3>& vertices, const glm::vec3& position) {
			const auto iter = std::ranges::find(vertices.begin(),
				vertices.end(), position);
			if (iter == vertices.end()) {
				return static_cast<uint16_t>(vertices.size());
			}
			else {
				return static_cast<uint16_t>(std::distance(vertices.begin(), iter));
			}
		}

		static uint16_t getIndexAndAdd(std::vector<glm::vec3>& vertices, glm::vec3 position) {
			const auto index = getIndex(vertices, position);
			if (index == vertices.size())
				vertices.push_back(position);
			return index;
		}
		
		static glm::vec3 getSurfaceNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
			const glm::vec3 u = b - a; 
			const glm::vec3 v = c - a; 
			const glm::vec3 normal{ 
				(u.y * v.z) - (u.z * v.y),
				(u.z * v.x) - (u.x * v.z),
				(u.x * v.y) - (u.y * v.x)
			};

			return glm::normalize(normal);
		}

	}

	// much of the code for the UVSphere implementation is from
	// https://gist.github.com/Pikachuxxxx/5c4c490a7d7679824e0e18af42918efc
	// everything else is written by me

	namespace UVSphere {

		static SphereData createSphere(int rings) {
			SphereData sphereData{};

			if (rings < 2) {
				rings = 2;
			}

			const int segments = rings * 2;
			constexpr float radius = 1.0f;

			float lengthInv = 1.0f / radius;

			const float deltaLatitude = glm::pi<float>() / static_cast<float>(rings);
			const float deltaLongitude = 2 * glm::pi<float>() / static_cast<float>(segments);
			float latitudeAngle = 0.0f, longitudeAngle = 0.0f;

			for (int i = 0; i <= rings; ++i)
			{
				latitudeAngle = glm::pi<float>() / 2.0f - i * deltaLatitude;
				const float xy = radius * cosf(latitudeAngle);
				const float z = radius * sinf(latitudeAngle);
				for (int j = 0; j <= segments; ++j)
				{
					longitudeAngle = j * deltaLongitude;
					const glm::vec3 vertex{ xy * cosf(longitudeAngle), xy * sinf(longitudeAngle), z };
					sphereData.m_Vertices.push_back(vertex);
				}
			}

			unsigned int k1 = 0, k2 = 0;
			for (int i = 0; i < rings; ++i)
			{
				k1 = i * (segments + 1);
				k2 = k1 + segments + 1;
				for (int j = 0; j < segments; ++j, ++k1, ++k2)
				{
					if (i != 0)
					{
						sphereData.m_Indices.push_back(k1);
						sphereData.m_Indices.push_back(k2);
						sphereData.m_Indices.push_back(k1 + 1);
					}

					if (i != (rings - 1))
					{
						sphereData.m_Indices.push_back(k1 + 1);
						sphereData.m_Indices.push_back(k2);
						sphereData.m_Indices.push_back(k2 + 1);
					}
				}
			}

			for (auto& vertex : sphereData.m_Vertices) {
				// a vertex on a sphere is its own normal
				vertex.m_Normal = vertex.m_Position;
				vertex.m_UV = { vertex.m_Normal.x / 2.0f + 0.5f, vertex.m_Normal.y / 2.0f + 0.5f };
			}

			return sphereData;
		}

	}

	namespace IcoSphere {

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

		static SphereData createSphere(int resolution) {
			SphereData sphereData{};

			sphereData.m_Indices = m_IcosahedronIndices;

			std::vector<glm::vec3> inVerts = m_IcosahedronPositions;
			std::vector<uint16_t> inIndices = sphereData.m_Indices;

			if (resolution == 0) {
				for (auto& position : inVerts) {
					position = glm::normalize(position);
				}
			}

			for (int j = 0; j < resolution; j++) {
				std::vector<glm::vec3> nVerts{};
				std::vector<uint16_t> nIndices{};
				for (int i = 0; i < inIndices.size(); i += 3) {
					glm::vec3 a = inVerts[inIndices[i]];
					auto aIndex = SphereUtility::getIndexAndAdd(nVerts, a);

					glm::vec3 b = inVerts[inIndices[i + 1]];
					auto bIndex = SphereUtility::getIndexAndAdd(nVerts, b);

					glm::vec3 c = inVerts[inIndices[i + 2]];
					auto cIndex = SphereUtility::getIndexAndAdd(nVerts, c);

					glm::vec3 d = b + ((a - b) / 2.0f);
					auto dIndex = SphereUtility::getIndexAndAdd(nVerts, d);

					glm::vec3 e = c + ((b - c) / 2.0f);
					auto eIndex = SphereUtility::getIndexAndAdd(nVerts, e);

					glm::vec3 f = c + ((a - c) / 2.0f);
					auto fIndex = SphereUtility::getIndexAndAdd(nVerts, f);
					
					std::vector<uint16_t> _indices{
						aIndex, dIndex, fIndex,
						dIndex, bIndex, eIndex,
						eIndex, fIndex, dIndex,
						fIndex, eIndex, cIndex
					};

					nIndices.insert(nIndices.begin(), _indices.begin(), _indices.end());
				}
				inVerts = nVerts;
				inIndices = nIndices;
			}

			for (auto& pos : inVerts) {
				sphereData.m_Vertices.push_back({ glm::normalize(pos) });
			}

			sphereData.m_Indices = inIndices;

			for (auto& vertex : sphereData.m_Vertices) {
				// a vertex on a sphere is its own normal
				vertex.m_Normal = vertex.m_Position;
				vertex.m_UV = { vertex.m_Normal.x / 2.0f + 0.5f, vertex.m_Normal.y / 2.0f + 0.5f };
			}

			return sphereData;
		}

	}

	namespace QuadSphere {
		
		static SphereData createSphere(int resolution, bool useIndexing) {
			SphereData sphereData{};
			std::vector<glm::vec3> inVerts = m_CubePositions;

			if (useIndexing) {
				for (int i = 0; i < m_CubePositions.size(); i += 4) {
					std::vector _indices{
						(uint16_t)i, (uint16_t)(i + 1), (uint16_t)(i + 2),
						(uint16_t)(i + 2), (uint16_t)(i + 3), (uint16_t)i
					};
					sphereData.m_Indices.insert(sphereData.m_Indices.end(),
						_indices.begin(), _indices.end());
				}

				std::vector<uint16_t> inIndices = sphereData.m_Indices;
				for (int j = 0; j < resolution; j++) {
					std::vector<glm::vec3> nVerts{};
					std::vector<uint16_t> nIndices{};

					for (int i = 0; i < inIndices.size(); i += 6) {
						auto a = inVerts[inIndices[i]];
						auto aIndex = SphereUtility::getIndexAndAdd(nVerts, a);

						auto b = inVerts[inIndices[i + 1]];
						auto bIndex = SphereUtility::getIndexAndAdd(nVerts, b);

						auto c = inVerts[inIndices[i + 2]];
						auto cIndex = SphereUtility::getIndexAndAdd(nVerts, c);

						auto d = inVerts[inIndices[i + 4]];
						auto dIndex = SphereUtility::getIndexAndAdd(nVerts, d);

						auto e = b + ((a - b) / 2.0f);
						auto eIndex = SphereUtility::getIndexAndAdd(nVerts, e);

						auto f = c + ((b - c) / 2.0f);
						auto fIndex = SphereUtility::getIndexAndAdd(nVerts, f);

						auto g = c + ((d - c) / 2.0f);
						auto gIndex = SphereUtility::getIndexAndAdd(nVerts, g);

						auto h = d + ((a - d) / 2.0f);
						auto hIndex = SphereUtility::getIndexAndAdd(nVerts, h);

						auto _i = c + ((a - c) / 2.0f);
						auto iIndex = SphereUtility::getIndexAndAdd(nVerts, _i);

						std::vector _indices{
							aIndex, eIndex, iIndex, iIndex, hIndex, aIndex,
							eIndex, bIndex, fIndex, fIndex, iIndex, eIndex,
							iIndex, fIndex, cIndex, cIndex, gIndex, iIndex,
							hIndex, iIndex, gIndex, gIndex, dIndex, hIndex
						};

						nIndices.insert(nIndices.end(), _indices.begin(), _indices.end());
					}

					inVerts = nVerts;
					inIndices = nIndices;
				}

				sphereData.m_Indices = inIndices;
			}
			else {
				std::vector<glm::vec3> nVerts{}; 
				for (int i = 0; i < inVerts.size(); i += 4) {
					auto a = inVerts[i];
					auto b = inVerts[i + 1];
					auto c = inVerts[i + 2];
					auto d = inVerts[i + 3];
					std::vector verts{ a, b, c, c, d, a };
					nVerts.insert(nVerts.begin(), verts.begin(), verts.end()); 
				}
				inVerts = nVerts; 
				for (int j = 0; j < resolution; j++) {
					std::vector<glm::vec3> nVerts{};

					for (int i = 0; i < inVerts.size(); i += 6) {
						auto a = inVerts[i];
						auto b = inVerts[i + 1];
						auto c = inVerts[i + 2];
						auto d = inVerts[i + 4];
						auto e = b + ((a - b) / 2.0f);
						auto f = c + ((b - c) / 2.0f);
						auto g = c + ((d - c) / 2.0f);
						auto h = d + ((a - d) / 2.0f);
						auto _i = c + ((a - c) / 2.0f);

						std::vector<glm::vec3> verts{
							a, e, _i, _i, h, a,
							e, b, f, f, _i, e,
							_i, f, c, c, g, _i,
							h, _i, g, g, d, h
						};

						nVerts.insert(nVerts.end(), verts.begin(), verts.end());
					}

					inVerts = nVerts;
				}
			}

			sphereData.m_Vertices = std::vector(inVerts.size(), SphereVertex{});

			if (useIndexing) {
				for (int i = 0; i < inVerts.size(); i++) {
					auto& vertex = sphereData.m_Vertices[i];
					vertex.m_Position = glm::normalize(inVerts[i]);
					vertex.m_Normal = vertex.m_Position;
					vertex.m_UV = { vertex.m_Position.x / 2.0f + 0.5f, vertex.m_Position.y / 2.0f + 0.5f };
				}
			}
			else {
				for (int i = 0; i < sphereData.m_Vertices.size(); i++) {
					auto& vertex = sphereData.m_Vertices[i];

					constexpr auto M_PI = glm::pi<float>(); 

					vertex.m_Position = glm::normalize(inVerts[i]);
					vertex.m_Normal = vertex.m_Position;
					vertex.m_UV = {
						1.0f - glm::clamp((std::atan2(vertex.m_Position.z, 
						vertex.m_Position.x) + M_PI) / (2.0f * M_PI), 0.0f, 1.0f),
						1.0f - glm::clamp(((std::atan(-vertex.m_Position.y / 
							glm::length(glm::vec2{vertex.m_Position.x, vertex.m_Position.z})) + (M_PI / 2.0f)) 
							/ M_PI), 0.0f, 1.0f)
					};
				}
			}

			return sphereData; 
		}
		
	}

}