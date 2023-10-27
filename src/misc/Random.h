#ifndef RANDOM_H
#define RANDOM_H

#include "../wrapper/Float.h"

//#define RAND_USE_LFSR

class Random {
public:
#ifdef RAND_USE_OLD
#ifdef WIN32
	using SeedType = uint32_t;
#else
	using SeedType = uint64_t;
#endif // WIN32
#else
	using SeedType = uint64_t;
#endif // RAND_USE_LFSR

public:
	Random() {};
	~Random() {};

#ifdef RAND_USE_OLD
	//static thread_local Random::SeedType Seed;
#ifdef WIN32
	static thread_local uint32_t Seed;
#else
	static thread_local uint64_t Seed;
#endif // WIN32
#else
	static thread_local Random::SeedType Seed;
#endif // RAND_USE_OLD

	/// <summary>
	/// Returns a 32 bit unsigned int - a 64 bit one is unnecessary
	/// </summary>
	/// <param name="bitCount"></param>
	/// <returns></returns>
	static unsigned int RandomUInt(const unsigned int bitCount = 32);

	static Float RandomFloat(const Float min = 0, const Float max = 1);
	static int RandomInt(const int min, const int max);

	/// <summary>
	/// Random value in normal distribution with mean=0 and sd=1
	/// </summary>
	/// <returns>A value between -1 and 1</returns>
	static Float RandomFloatND();
};

#endif // !RANDOM_H
