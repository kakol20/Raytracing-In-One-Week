#include <limits>

#include "Random.h"

unsigned int Random::RandomUInt(const unsigned int& bitCount) {
	unsigned int count = bitCount > 32 ? 32 : bitCount;
	unsigned int out = 0;

	if (Random::Seed == 0) Random::Seed = 0b1;

	for (int i = (int)count - 1; i >= 0; i--) {
		out = out | ((Random::Seed & 0b1) << i);

#ifdef WIN32
		Random::SeedType newBit = Random::Seed ^ (Random::Seed >> 1) ^ (Random::Seed >> 21) ^ (Random::Seed >> 31);
		newBit = newBit & 1;

		Random::Seed = (Random::Seed >> 1) | (newBit << 31);
#else
		Random::SeedType newBit = (Random::Seed) ^ (Random::Seed >> 59) ^ (Random::Seed >> 60) ^ (Random::Seed >> 62) ^ (Random::Seed >> 63);
		newBit = newBit & 1;

		Random::Seed = (Random::Seed >> 1) | (newBit << 63);
#endif // WIN32
	}

	return out;
}

Float Random::RandomFloat(const Float& min, const Float& max) {
	Float random = Float(Random::RandomUInt());
	random /= std::numeric_limits<unsigned int>::max();
	return (random * (max - min)) + min;
}