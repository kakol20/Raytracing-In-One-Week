#include "../../ext/oof/oof.h"
#include "../wrapper/Fastwrite.h"

#include "BlueNoise.h"

void BlueNoise::Generate(const int& sampleCount, const int& m) {
	FastWrite::Reset();
	FastWrite::Write("\a");

	m_samples.clear();
	m_samples.reserve(sampleCount);

	for (int i = 0; i < sampleCount; i++) {
		FastWrite::Reset();

		int candidateCount = (int)m_samples.size() * m + 1;

		Vector3D furthestPoint(0, 0);
		Float furthestDist = Float::MinVal;
		for (int j = 0; j < candidateCount; j++) {
			Vector3D currentPoint = Vector3D::RandomVector(0, 1, false);
			Float closestDist = Float::MaxVal;

			for (size_t k = 0; k < m_samples.size(); k++) {
				Float currentDist = Vector3D::ToroidalDistance(currentPoint, m_samples[k], Vector3D::Zero, Vector3D::One);

				if (currentDist < closestDist) closestDist = currentDist;
			}

			if (closestDist > furthestDist) {
				furthestPoint = currentPoint;
				furthestDist = closestDist;
			}
		}

		m_samples.push_back(furthestPoint);

		FastWrite::Write("Blue Noise Progress: " + std::to_string(m_samples.size()) + " / " + std::to_string(sampleCount));
	}
}

Vector3D& BlueNoise::operator[](size_t index) {
	size_t l_index = index % m_samples.size();
	return m_samples[l_index];
}
