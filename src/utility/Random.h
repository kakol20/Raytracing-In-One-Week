#pragma once

#include <cstdint>

class Random {
public:
	using SeedType = uint64_t;

	Random() {};
	~Random() {};

	static thread_local uint64_t Seed;

	/// <summary>
	/// Returns a 32 bit unsigned int - a 64 bit unsigned int is unnecessary
	/// </summary>
	/// <param name="bitCount"></param>
	/// <returns></returns>
	static unsigned int RandomUInt(const unsigned int& bitCount = 32);

	static float RandomFloat(const float& min = 0.f, const float& max = 1.f);
	static double RandomDouble(const double& min = 0., const double& max = 1.);
	static int RandomInt(const int& min, const int& max);
};