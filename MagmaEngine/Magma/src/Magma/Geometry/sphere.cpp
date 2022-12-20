#include "sphere.h"

namespace Magma {

	SphereGenStrategy::SphereGenStrategy(Sphere& sphere) : m_Sphere{sphere} {}

	UVSphereGenStrategy::UVSphereGenStrategy(Sphere& sphere) : SphereGenStrategy(sphere) {}

	uint32_t UVSphereGenStrategy::getMaxVertices() {
		return 100000;
	}

	void UVSphereGenStrategy::createVertices() {
		// TODO: make this accessible to creator
		int rings = 2;

		if (rings < 2) {
			rings = 2;
		}

		const int segments = rings * 2;
		constexpr float radius = 1.0f;

		const float deltaLatitude = glm::pi<float>() / static_cast<float>(rings);
		const float deltaLongitude = 2 * glm::pi<float>() / static_cast<float>(segments);

		for (int i = 0; i <= rings; ++i) {
			const float latitudeAngle = glm::pi<float>() / 2.0f - static_cast<float>(i) * deltaLatitude;
			const float xy = radius * cosf(latitudeAngle);
			const float z = radius * sinf(latitudeAngle);
			for (int j = 0; j <= segments; ++j)
			{
				const float longitudeAngle = static_cast<float>(j) * deltaLongitude;
				const glm::vec3 position{ xy * cosf(longitudeAngle), xy * sinf(longitudeAngle), z };
				m_Sphere.getVertices().push_back({ position });
			}
		}

		uint16_t k1 = 0, k2 = 0;
		for (int i = 0; i < rings; ++i) {
			k1 = static_cast<uint16_t>(i * (segments + 1));
			k2 = k1 + static_cast<uint16_t>(segments + 1);
			for (int j = 0; j < segments; ++j, ++k1, ++k2) {
				if (i != 0) {
					m_Sphere.getIndices().push_back(k1);
					m_Sphere.getIndices().push_back(k2);
					m_Sphere.getIndices().push_back(k1 + 1);
				}

				if (i != (rings - 1)) {
					m_Sphere.getIndices().push_back(k1 + 1);
					m_Sphere.getIndices().push_back(k2);
					m_Sphere.getIndices().push_back(k2 + 1);
				}
			}
		}
	}

	void UVSphereGenStrategy::createUVs() {
		constexpr auto M_PI = glm::pi<float>();
		for (auto& [pos, n, UV] : m_Sphere.getVertices()) {
			UV = { 1.0f - glm::clamp((std::atan2(pos.z, pos.x) + M_PI) / (2.0f * M_PI), 0.0f, 1.0f),
				1.0f - glm::clamp(((std::atan(-pos.y / glm::length(glm::vec2{pos.x, pos.z})) + (M_PI / 2.0f))
				/ M_PI), 0.0f, 1.0f) };
		}

		SphereUtility::duplicateVertices(m_Sphere.getVertices());
	}

	IcoSphereGenStrategy::IcoSphereGenStrategy(Sphere& sphere) : SphereGenStrategy(sphere) {}

	uint32_t IcoSphereGenStrategy::getMaxVertices() {
		return 100000;
	}

	void IcoSphereGenStrategy::createVertices() {
		m_Sphere.getVertices() = std::vector<Vertex>{ getMaxVertices() };

		std::vector<glm::vec3> inPositions = m_IcosahedronPositions;

		if (m_Sphere.getResolution() == 0) {
			for (auto& position : inPositions) {
				position = glm::normalize(position);
			}
		}

		if (m_Sphere.doesGenerateIndices()) {
			m_Sphere.getIndices() = m_IcosahedronIndices;

			std::vector<uint16_t> inIndices = m_Sphere.getIndices();
			for (int j = 0; j < static_cast<int>(m_Sphere.getResolution()); j++) {
				std::vector<glm::vec3> nPositions{};
				std::vector<uint16_t> nIndices{};
				for (int i = 0; i < static_cast<int>(inIndices.size()); i += 3) {
					glm::vec3 positionA = inPositions[inIndices[i]];
					glm::vec3 positionB = inPositions[inIndices[i + 1]];
					glm::vec3 positionC = inPositions[inIndices[i + 2]];
					glm::vec3 positionD = positionB + (positionA - positionB) / 2.0f;
					glm::vec3 positionE = positionC + (positionB - positionC) / 2.0f;
					glm::vec3 positionF = positionC + (positionA - positionC) / 2.0f;

					auto aIndex = SphereUtility::getIndexAndAdd(nPositions, positionA);
					auto bIndex = SphereUtility::getIndexAndAdd(nPositions, positionB);
					auto cIndex = SphereUtility::getIndexAndAdd(nPositions, positionC);
					auto dIndex = SphereUtility::getIndexAndAdd(nPositions, positionD);
					auto eIndex = SphereUtility::getIndexAndAdd(nPositions, positionE);
					auto fIndex = SphereUtility::getIndexAndAdd(nPositions, positionF);

					std::vector indices{
						aIndex, dIndex, fIndex,
						dIndex, bIndex, eIndex,
						eIndex, fIndex, dIndex,
						fIndex, eIndex, cIndex
					};

					nIndices.insert(nIndices.begin(), indices.begin(), indices.end());
				}

				inPositions = nPositions;
				inIndices = nIndices;
			}

			for (auto& pos : inPositions) {
				m_Sphere.getVertices().emplace_back(Vertex{ glm::normalize(pos) });
			}

			m_Sphere.getIndices() = inIndices;
		} else {
			inPositions.clear();

			for (auto& index : m_IcosahedronIndices) {
				inPositions.emplace_back(m_IcosahedronPositions[index]); 
			}

			for (int i = 0; i < static_cast<int>(m_Sphere.getResolution()); i++) {
				std::vector<glm::vec3> nPositions{};
				for (int j = 0; j < static_cast<int>(inPositions.size()); j += 3) {
					glm::vec3 positionA = inPositions[j];
					glm::vec3 positionB = inPositions[j + 1];
					glm::vec3 positionC = inPositions[j + 2];
					glm::vec3 positionD = positionB + (positionA - positionB) / 2.0f;
					glm::vec3 positionE = positionC + (positionB - positionC) / 2.0f;
					glm::vec3 positionF = positionC + (positionA - positionC) / 2.0f;

					std::vector positions{
						positionA, positionD, positionF,
						positionD, positionB, positionE,
						positionE, positionF, positionD,
						positionF, positionE, positionC
					};

					nPositions.insert(nPositions.end(), positions.begin(), positions.end()); 
				}

				inPositions = nPositions; 
			}

			for (int i = 0; i < static_cast<int>(inPositions.size()); i++) {
				m_Sphere.getVertices()[i] = { glm::normalize(inPositions[i]) };
			}
		}

		// set vertex count
		m_Sphere.setVertexCount(static_cast<uint32_t>(inPositions.size()));
	}

	void IcoSphereGenStrategy::createUVs() {
		constexpr auto M_PI = glm::pi<float>();
		for (auto& [pos, n, UV] : m_Sphere.getVertices()) {
			UV = { 1.0f - glm::clamp((std::atan2(pos.z, pos.x) + M_PI) / (2.0f * M_PI), 0.0f, 1.0f),
				1.0f - glm::clamp(((std::atan(-pos.y / glm::length(glm::vec2{pos.x, pos.z})) + (M_PI / 2.0f))
				/ M_PI), 0.0f, 1.0f) };
		}

		// TODO: broken for icosphere 
		SphereUtility::duplicateVertices(m_Sphere.getVertices());
	}

	QuadSphereGenStrategy::QuadSphereGenStrategy(Sphere& sphere) : SphereGenStrategy(sphere) {}

	uint32_t QuadSphereGenStrategy::getMaxVertices() {
		// vertices > indexing
		return 36 * static_cast<uint32_t>(glm::pow(4, m_Sphere.getMaxResolution()));
	}

	void QuadSphereGenStrategy::createVertices() {
		m_Sphere.getVertices() = std::vector<Vertex>{ getMaxVertices() };
		std::vector<glm::vec3> inPositions = m_CubePositions;

		if (m_Sphere.doesGenerateIndices()) {
			for (int i = 0; i < static_cast<int>(m_CubePositions.size()); i += 4) {
				std::vector indices{
					static_cast<uint16_t>(i), static_cast<uint16_t>(i + 1),
					static_cast<uint16_t>(i + 2), static_cast<uint16_t>(i + 2),
					static_cast<uint16_t>(i + 3), static_cast<uint16_t>(i)
				};
				m_Sphere.getIndices().insert(m_Sphere.getIndices().end(),
					indices.begin(), indices.end());
			}

			std::vector<uint16_t> inIndices = m_Sphere.getIndices();
			for (int j = 0; j < static_cast<int>(m_Sphere.getResolution()); j++) {
				std::vector<glm::vec3> nPositions{};
				std::vector<uint16_t> nIndices{};

				for (int i = 0; i < static_cast<int>(inIndices.size()); i += 6) {
					auto a = inPositions[inIndices[i]];
					auto aIndex = SphereUtility::getIndexAndAdd(nPositions, a);

					auto b = inPositions[inIndices[i + 1]];
					auto bIndex = SphereUtility::getIndexAndAdd(nPositions, b);

					auto c = inPositions[inIndices[i + 2]];
					auto cIndex = SphereUtility::getIndexAndAdd(nPositions, c);

					auto d = inPositions[inIndices[i + 4]];
					auto dIndex = SphereUtility::getIndexAndAdd(nPositions, d);

					auto e = b + ((a - b) / 2.0f);
					auto eIndex = SphereUtility::getIndexAndAdd(nPositions, e);

					auto f = c + ((b - c) / 2.0f);
					auto fIndex = SphereUtility::getIndexAndAdd(nPositions, f);

					auto g = c + ((d - c) / 2.0f);
					auto gIndex = SphereUtility::getIndexAndAdd(nPositions, g);

					auto h = d + ((a - d) / 2.0f);
					auto hIndex = SphereUtility::getIndexAndAdd(nPositions, h);

					auto _i = c + ((a - c) / 2.0f);
					auto iIndex = SphereUtility::getIndexAndAdd(nPositions, _i);

					std::vector indices{
						aIndex, eIndex, iIndex, iIndex, hIndex, aIndex,
						eIndex, bIndex, fIndex, fIndex, iIndex, eIndex,
						iIndex, fIndex, cIndex, cIndex, gIndex, iIndex,
						hIndex, iIndex, gIndex, gIndex, dIndex, hIndex
					};

					nIndices.insert(nIndices.end(), indices.begin(), indices.end());
				}

				inPositions = nPositions;
				inIndices = nIndices;
			}

			m_Sphere.getIndices() = inIndices;
		}
		else {
			std::vector<glm::vec3> nPositions{};
			for (int i = 0; i < static_cast<int>(inPositions.size()); i += 4) {
				const auto& a = inPositions[i];
				const auto& c = inPositions[i + 2];
				std::vector verts{ a, inPositions[i + 1], c, c, inPositions[i + 3], a };
				nPositions.insert(nPositions.begin(), verts.begin(), verts.end());
			}

			inPositions = nPositions;

			for (int j = 0; j < static_cast<int>(m_Sphere.getResolution()); j++) {
				nPositions.clear();
				for (int i = 0; i < static_cast<int>(inPositions.size()); i += 6) {
					const auto positionA = inPositions[i];
					const auto positionB = inPositions[i + 1];
					const auto positionC = inPositions[i + 2];
					const auto positionD = inPositions[i + 4];
					const auto positionE = positionB + (positionA - positionB) / 2.0f;
					const auto positionF = positionC + (positionB - positionC) / 2.0f;
					const auto positionG = positionC + (positionD - positionC) / 2.0f;
					const auto positionH = positionD + (positionA - positionD) / 2.0f;
					const auto positionI = positionC + (positionA - positionC) / 2.0f;
					std::vector positions{
						positionA, positionE, positionI, positionI, positionH, positionA,
						positionE, positionB, positionF, positionF, positionI, positionE,
						positionI, positionF, positionC, positionC, positionG, positionI,
						positionH, positionI, positionG, positionG, positionD, positionH
					};
					nPositions.insert(nPositions.end(), positions.begin(), positions.end());
				}

				inPositions = nPositions;
			}
		}

		for (int i = 0; i < static_cast<int>(inPositions.size()); i++) {
			m_Sphere.getVertices()[i].m_Position = glm::normalize(inPositions[i]);
		}

		// set vertex count
		m_Sphere.setVertexCount(static_cast<uint32_t>(inPositions.size()));
	}

	void QuadSphereGenStrategy::createUVs() {
		constexpr auto M_PI = glm::pi<float>();
		for (auto& [pos, n, UV] : m_Sphere.getVertices()) {
			UV = { 1.0f - glm::clamp((std::atan2(pos.z, pos.x) + M_PI) / (2.0f * M_PI), 0.0f, 1.0f),
				1.0f - glm::clamp(((std::atan(-pos.y / glm::length(glm::vec2{pos.x, pos.z})) + (M_PI / 2.0f))
				/ M_PI), 0.0f, 1.0f) };
		}

		SphereUtility::duplicateVertices(m_Sphere.getVertices());
	}

	Sphere::Sphere(uint32_t maxResolution, SphereGenerationStrategy generationStrategy) :
		m_MaxResolution{ maxResolution }, m_Resolution{ maxResolution } {
		setGenerationStrategy(generationStrategy); 
	}

	std::shared_ptr<Buffer> Sphere::getBuffer() const {
		return m_Buffer;
	}

	uint32_t Sphere::getMaxResolution() const {
		return m_MaxResolution;
	}

	uint32_t Sphere::getResolution() const {
		return m_Resolution;
	}

	uint32_t Sphere::getVertexCount() const {
		return m_VertexCount;
	}

	bool Sphere::doesGenerateIndices() const {
		return m_GenerateIndices;
	}

	bool Sphere::doesGenerateUVs() const {
		return m_GenerateUVs;
	}

	void Sphere::setGenerationStrategy(SphereGenerationStrategy generationStrategy) {
		switch (generationStrategy) {
			case SphereGenerationStrategy::UV_SPHERE:
				m_GenStrategy = std::make_unique<UVSphereGenStrategy>(*this);
				break;
			case SphereGenerationStrategy::ICO_SPHERE:
				m_GenStrategy = std::make_unique<IcoSphereGenStrategy>(*this);
				break;
			case SphereGenerationStrategy::QUAD_SPHERE:
				m_GenStrategy = std::make_unique<QuadSphereGenStrategy>(*this);
				break;
			default:
				throw std::invalid_argument("invalid sphere generation strategy: " + 
					std::to_string(static_cast<int>(generationStrategy))); 
		}
	}

	void Sphere::setResolution(uint32_t resolution) {
		m_Resolution = resolution; 
	}

	void Sphere::setVertexCount(uint32_t vertexCount) {
		m_VertexCount = vertexCount;
	}

	void Sphere::create(RenderCore& renderCore) {
		if (m_Buffer == nullptr) {
			// TODO: calculate max number of vertices
			m_Buffer = renderCore.createBuffer(static_cast<int64_t>(sizeof(Vertex)) *
				m_GenStrategy->getMaxVertices(), BufferUsage::VERTEX);
			m_Buffer->map();
		}

		m_GenStrategy->createVertices();

		if (m_GenerateNormals) {
			createNormals();
		}

		if (m_GenerateUVs) {
			m_GenStrategy->createUVs();
		}

		// set buffer data
		m_Buffer->setData(m_Vertices.data(), sizeof(Vertex) * m_Vertices.size());
	}

	void Sphere::createNormals() {
		for (auto& vertex : m_Vertices) {
			vertex.m_Normal = vertex.m_Position;
		}
	}



}