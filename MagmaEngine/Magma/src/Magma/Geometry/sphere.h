#pragma once

#include "cube.h"

namespace Magma {

	struct SphereVertex {
		SphereVertex(glm::vec3 pos) : pos{ pos }, color{ 1.0f } {}

		glm::vec3 pos, color;
	};

	struct SphereData {
		std::vector<SphereVertex> verts{};
		std::vector<uint16_t> indices{};
	};

	namespace SphereUtility {

		static uint16_t getIndex(std::vector<glm::vec3>& verts, glm::vec3& pos) {
			auto iter = std::find(verts.begin(), verts.end(), pos);
			if (iter == verts.end()) return verts.size();
			else return std::distance(verts.begin(), iter);
		}

		static uint16_t getIndexAndAdd(std::vector<glm::vec3>& verts, glm::vec3 pos) {
			auto index = getIndex(verts, pos);
			if (index == verts.size())
				verts.push_back(pos);
			return index;
		}

	}

	// much of the code for the UVSphere implementation is from
	// https://gist.github.com/Pikachuxxxx/5c4c490a7d7679824e0e18af42918efc
	// everything else is written by me

	namespace UVSphere {

		static SphereData createSphere(int rings) {

			SphereData sphereData{};

			if (rings < 2)
				rings = 2;

			int segments = rings * 2;
			float radius = 1.0f;

			float lengthInv = 1.0f / radius;

			float deltaLatitude = glm::pi<float>() / rings;
			float deltaLongitude = 2 * glm::pi<float>() / segments;
			float latitudeAngle, longitudeAngle;

			for (int i = 0; i <= rings; ++i)
			{
				latitudeAngle = glm::pi<float>() / 2.0f - i * deltaLatitude;
				float xy = radius * cosf(latitudeAngle);
				float z = radius * sinf(latitudeAngle);

				for (int j = 0; j <= segments; ++j)
				{
					longitudeAngle = j * deltaLongitude;

					glm::vec3 vertex{ xy * cosf(longitudeAngle), xy * sinf(longitudeAngle), z };
					sphereData.verts.push_back(vertex);
				}
			}

			unsigned int k1, k2;
			for (int i = 0; i < rings; ++i)
			{
				k1 = i * (segments + 1);
				k2 = k1 + segments + 1;
				for (int j = 0; j < segments; ++j, ++k1, ++k2)
				{
					if (i != 0)
					{
						sphereData.indices.push_back(k1);
						sphereData.indices.push_back(k2);
						sphereData.indices.push_back(k1 + 1);
					}

					if (i != (rings - 1))
					{
						sphereData.indices.push_back(k1 + 1);
						sphereData.indices.push_back(k2);
						sphereData.indices.push_back(k2 + 1);
					}
				}
			}

			return sphereData;
		}

	}

	namespace IcoSphere {

		const float t = (1.0f + sqrt(5.0f)) / 2.0f;

		const std::vector<glm::vec3> icosahedronVerts{
			{t, 1.0f, 0}, {-t, 1.0f, 0.0f}, { t, -1.0f, 0.0f },
			{-t, -1.0f, 0.0f}, {1.0f, 0.0f, t}, {1.0f, 0.0f, -t},
			{-1.0f, 0.0f, t}, {-1.0f, 0.0f, -t}, {0.0f, t, 1.0f},
			{0.0f, -t, 1.0f}, {0.0f, t, -1.0f}, {0.0f, -t, -1.0f}
		};

		const std::vector<uint16_t> icosahedronIndices{
			0, 8, 4, 0, 5, 10, 2, 4, 9, 2, 11, 5, 1, 6, 8, 1, 10, 7, 3, 9, 6, 3, 7, 11, 0, 10, 8, 1, 8, 10,
			2, 9, 11, 3, 9, 11, 4, 2, 0, 5, 0, 2, 6, 1, 3, 7, 3, 1, 8, 6, 4, 9, 4, 6, 10, 5, 7, 11, 7, 5
		};

		static SphereData createSphere(int resolution) {
			SphereData sphereData{};

			sphereData.indices = icosahedronIndices;

			std::vector<glm::vec3> inVerts = icosahedronVerts;
			std::vector<uint16_t> inIndices = sphereData.indices;

			if (resolution == 0)
				for (int i = 0; i < inVerts.size(); i++)
					inVerts[i] = glm::normalize(inVerts[i]);

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
				sphereData.verts.push_back({ glm::normalize(pos) });
			}

			sphereData.indices = inIndices;

			return sphereData;
		}

	}

	namespace QuadSphere {
		
		static SphereData createSphere(int resolution) {
			SphereData sphereData{};

			for (int i = 0; i < cubeVerts.size(); i += 4) {
				std::vector<uint16_t> _indices{
					(uint16_t)i, (uint16_t)(i + 1), (uint16_t)(i + 2), 
					(uint16_t)(i + 2), (uint16_t)(i + 3), (uint16_t)i
				};
				sphereData.indices.insert(sphereData.indices.end(), 
					_indices.begin(), _indices.end());
			}

			std::vector<glm::vec3> inVerts = cubeVerts;
			std::vector<uint16_t> inIndices = sphereData.indices;

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

					std::vector<uint16_t> _indices{
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

			for (auto& pos : inVerts) {
				sphereData.verts.push_back({ glm::normalize(pos) });
			}

			sphereData.indices = inIndices;

			return sphereData; 
		}
		
	}

}