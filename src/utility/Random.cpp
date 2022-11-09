#include <limits>

#include "Random.h"

unsigned int Random::RandomUInt(const unsigned int& bitCount) {
	unsigned int count = bitCount > 32 ? 32 : bitCount;
	unsigned int out = 0;

	if (Random::Seed == 0) Random::Seed = 0b1;

	for (int i = (int)count - 1; i >= 0; i--) {
		out = out | ((Random::Seed & 0b1) << i);

		Random::SeedType newBit = (Random::Seed) ^ (Random::Seed >> 59) ^ (Random::Seed >> 60) ^ (Random::Seed >> 62) ^ (Random::Seed >> 63);
		newBit = newBit & 1;

		Random::Seed = (Random::Seed >> 1) | (newBit << 63);
	}

	return out;
}

float Random::RandomFloat(const float& min, const float& max) {
	float random = static_cast<float>(Random::RandomUInt());
	random /= std::numeric_limits<unsigned int>::max();
	return (random * (max - min)) + min;
}

int Random::RandomInt(const int& min, const int& max) {
	int distance = (max - min) + 1;
	return (Random::RandomUInt() % distance) + min;
}