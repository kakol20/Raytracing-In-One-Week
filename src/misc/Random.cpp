#include <limits>
//#include <cstdlib>

#include "Random.h"

thread_local Random::SeedType Random::Seed = 0xACE1u;

unsigned int Random::RandomUInt(const unsigned int bitCount) {
	unsigned int count = bitCount > 32 ? 32 : bitCount;
	unsigned int out = 0;

	if (Random::Seed == 0) Random::Seed = 0b1;

#ifdef RAND_USE_LFSR
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
#else
	//out = rand();
// TODO: Chnage random algorithm https://gist.github.com/kakol20/8a95db78e2fa32ede6021e60db107a4e
	Random::Seed = Random::Seed * 1103515245 + 12345;
	out = (unsigned int)(Random::Seed / 65536) % 32768;
#endif // RAND_USE_LFSR

	return out;
}

Float Random::RandomFloat(const Float min, const Float max) {
	Float random = Float(Random::RandomUInt());

#ifdef RAND_USE_LFSR
	random /= std::numeric_limits<unsigned int>::max();
#else
	random /= 32767;
#endif // RAND_USE_LFSR

	return (random * (max - min)) + min;
}

int Random::RandomInt(const int min, const int max) {
	int distance = (max - min) + 1;
	return (Random::RandomUInt() % distance) + min;
}

Float Random::RandomFloatND() {
	Float theta = Float::TAU * Random::RandomFloat(0, 1);
	Float rho = Float::Sqrt(Float::Log(Random::RandomFloat(0, 1)) * -2);
	return Float::Clamp(rho * Float::Cos(theta), -1, 1);

	//return Random::RandomFloat(-1, 1);
}