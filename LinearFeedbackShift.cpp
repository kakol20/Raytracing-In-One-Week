#include "LinearFeedbackShift.h"

#include <limits>

LinearFeedbackShift::LinearFeedbackShift() {
}

/// <summary>
/// Get random float between 0 and 1
/// </summary>
/// <param name="bitCount"></param>
/// <returns></returns>
float LinearFeedbackShift::RandFloat(unsigned int bitCount) {
	//unsigned int state = seed;
	unsigned int count = bitCount > 32 ? 32 : bitCount;

	// get max
	unsigned int max = 0;
	for (int i = count - 1; i >= 0; i--) {
		max = max | (0b1 << i);
	}

	unsigned int rand = LinearFeedbackShift::RandUInt(count);

	return (float)rand / (float)max;
}

/// <summary>
/// Get random float between min and max
/// </summary>
/// <param name="min"></param>
/// <param name="max"></param>
/// <param name="bitCount"></param>
/// <returns></returns>
float LinearFeedbackShift::RandFloatRange(const float min, const float max, unsigned int bitCount) {
	float difference = max - min;
	float random = LinearFeedbackShift::RandFloat(bitCount);
	return (difference * random) + min;
}

/// <summary>
/// Get random n-bit int
/// </summary>
/// <param name="bitCount"></param>
/// <returns></returns>
int LinearFeedbackShift::RandInt(const unsigned int bitCount) {
	int out = 0;

	unsigned int count = bitCount > 32 ? 32 : bitCount;
	count = count < 2 ? 2 : bitCount;

	unsigned int negative = RandUInt(1);

	for (int i = bitCount - 2; i >= 0; i--) {
		out = out + (LinearFeedbackShift::RandUInt(1) << i);
	}

	if (negative == 0b1) out = -out;

	return out;
}

/// <summary>
/// Get random 32-bit unsigned int
/// </summary>
/// <returns></returns>
unsigned int LinearFeedbackShift::RandUInt() {
	return LinearFeedbackShift::RandUInt(32);
}

/// <summary>
/// Get random n-bit unsigned int
/// </summary>
/// <param name="bitCount"></param>
/// <returns></returns>
unsigned int LinearFeedbackShift::RandUInt(const unsigned int bitCount) {
	unsigned int count = bitCount > 32 ? 32 : bitCount;
	unsigned int out = 0;

	std::mutex mtx;

	if (LinearFeedbackShift::Seed == 0) {
		std::time_t current_time = time(0);
		LinearFeedbackShift::Seed = static_cast<unsigned int>(current_time);
	}

	while (!mtx.try_lock());
	for (int i = bitCount - 1; i >= 0; i--) {
		out = out | ((LinearFeedbackShift::Seed & 0b1) << i);

		unsigned int newBit = LinearFeedbackShift::Seed ^ (LinearFeedbackShift::Seed >> 1) ^ (LinearFeedbackShift::Seed >> 2) ^ (LinearFeedbackShift::Seed >> 22);
		newBit = newBit & 1;

		LinearFeedbackShift::Seed = (LinearFeedbackShift::Seed >> 1) | (newBit << 31);
	}
	mtx.unlock();
	return out;
}

LinearFeedbackShift::~LinearFeedbackShift() {
}