#include <limits>

#include "../maths/Vector3.h"
#include "Random.h"

#include "BlueNoise.h"

void BlueNoise::Generate(const int& sampleCount, const int& m) {
	m_samples.clear();
	m_samples.reserve(sampleCount);

	for (int i = 0; i < sampleCount; i++) {

		int candidateCount = (int)m_samples.size() * m + 1;

		sf::Vector2f furthestPoint = { 0.f, 0.f };
		float furthestDist = 0.f;
		for (int j = 0; j < candidateCount; j++) {
			//sf::Vector3f ranVec = rt::Vector3::RandomVector(0.f, 1.f);

			sf::Vector2f currentPoint = { Random::RandomFloat(), Random::RandomFloat() };
			//sf::Vector2f currentPoint = sf::Vector2f::RandomVector(0, 1, false);
			float closestDist = std::numeric_limits<float>().max();

			for (size_t k = 0; k < m_samples.size(); k++) {
				float currentDist = rt::Vector3::SqrToroidalDistance({ currentPoint.x, currentPoint.y, 0.f }, { m_samples[k].x, m_samples[k].y, 0.f }, rt::Vector3::Zero, rt::Vector3::One);
				//float currentDist = sf::Vector2f::ToroidalDistance(currentPoint, m_samples[k], sf::Vector2f::Zero, sf::Vector2f::One);

				if (currentDist < closestDist) closestDist = currentDist;
			}

			if (closestDist > furthestDist) {
				furthestPoint = currentPoint;
				furthestDist = closestDist;
			}
		}

		m_samples.push_back(furthestPoint);
	}
}

sf::Vector2f& BlueNoise::operator[](size_t index) {
	size_t l_index = index % m_samples.size();
	return m_samples[l_index];
}
