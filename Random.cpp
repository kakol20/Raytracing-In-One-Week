#include <cmath>

#include "Random.h"

float Random::RandFloat() {
	//unsigned int max =
	unsigned int rand = Random::RandomUInt();

	return rand / (float)Random::MaxUInt;
}

float Random::RandFloatRange(const float from, const float to) {
	float factor = Random::RandFloat();
	return std::lerp(from, to, factor);
}

int Random::RandInt() {
	int out = (int)Random::RandomUInt(31);

	if (Random::RandomUInt(1) == 1) return -out;
	return out;
}

unsigned int Random::RandomUInt(const unsigned int bitCount) {
	// 32,22,2,1
	unsigned int count = bitCount > 32 ? 32 : bitCount;
	unsigned int out = 0;

	for (int i = (int)count - 1; i >= 0; i--) {
		out = out | ((Random::Seed & 0b1) << i);

		unsigned int newBit = Random::Seed ^ (Random::Seed >> 1) ^ (Random::Seed >> 21) ^ (Random::Seed >> 31);
		newBit = newBit & 1;

		Random::Seed = (Random::Seed >> 1) | (newBit << 31);
	}
	return out;
}