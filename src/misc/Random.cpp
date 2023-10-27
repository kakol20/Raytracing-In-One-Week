#include <limits>

#include "Random.h"

#ifdef RAND_USE_LFSR
//thread_local Random::SeedType Seed = 0xACE1u;
#ifdef WIN32
thread_local uint32_t Random::Seed = 0xACE1u;
#else
thread_local uint64_t Random::Seed = 0xACE1u;
#endif // WIN32
#else
thread_local uint32_t Random::Seed = 0xACE1u;
#endif // RAND_USE_LFSR

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
	unsigned int next = Random::Seed;
	next *= 1103515245;
	next += 12345;
	out = (unsigned int)(next / 65536) % 2048;

	next *= 1103515245;
	next += 12345;
	out <<= 10;
	out ^= (unsigned int)(next / 65536) % 1024;

	next *= 1103515245;
	next += 12345;
	out <<= 10;
	out ^= (unsigned int)(next / 65536) % 1024;

	Random::Seed = next;
#endif // RAND_USE_LFSR

	return out;
}

Float Random::RandomFloat(const Float min, const Float max) {
	Float random = Float(Random::RandomUInt());
	random /= std::numeric_limits<unsigned int>::max();
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