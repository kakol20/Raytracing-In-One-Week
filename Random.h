#pragma once

#include <limits>

class Random {
public:
	Random() {};
	~Random() {};

	static thread_local unsigned int Seed;

public:
	static float RandFloat();
	static float RandFloatRange(const float from, const float to);
	static int RandInt();
	static unsigned int RandomUInt(const unsigned int bitCount = 32);

	static const unsigned int MaxUInt = (std::numeric_limits<unsigned int>::max)();
};

