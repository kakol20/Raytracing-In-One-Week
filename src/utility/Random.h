#pragma once

#include <cstdint>

class Random {
public:
#ifdef WIN32
	using SeedType = uint32_t;
#else
	using SeedType = uint64_t;
#endif // WIN32

	Random() {};
	~Random() {};

#ifdef WIN32
	static thread_local uint32_t Seed;
#else
	static thread_local uint64_t Seed;
#endif // WIN32

	/// <summary>
	/// Returns a 32 bit unsigned int - a 64 bit unsigned int is unnecessary
	/// </summary>
	/// <param name="bitCount"></param>
	/// <returns></returns>
	static unsigned int RandomUInt(const unsigned int& bitCount = 32);

	static float RandomFloat(const float& min = 0.f, const float& max = 1.f);
	static int RandomInt(const int& min, const int& max);
};