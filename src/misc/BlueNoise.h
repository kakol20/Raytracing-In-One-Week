#ifndef BLUENOISE_H
#define BLUENOISE_H
#include <vector>

#include "../maths/Vector3D.h"

class BlueNoise {
public:
	BlueNoise() {};
	~BlueNoise() {};

	void Generate(const int& sampleCount, const int& m);

	size_t Size() const { return m_samples.size(); };
	Vector3D& operator[](size_t index);


private:
	std::vector<Vector3D> m_samples;
};

#endif // !BLUENOISE_H
