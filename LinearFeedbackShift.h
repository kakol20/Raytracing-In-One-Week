#ifndef LINEARFEEDBACKSHIFT_H
#define LINEARFEEDBACKSHIFT_H
#include <mutex>

class LinearFeedbackShift {
public:
	LinearFeedbackShift();

	static float RandFloat(unsigned int bitCount);
	static float RandFloatRange(const float min, const float max, unsigned int bitCount);
	static int RandInt(const unsigned int bitCount);
	static unsigned int RandUInt();
	static unsigned int RandUInt(const unsigned int bitCount);

	static unsigned int Seed;

	

	~LinearFeedbackShift();

private:
};

#endif // !LINEARFEADBACKSHIFT_H
