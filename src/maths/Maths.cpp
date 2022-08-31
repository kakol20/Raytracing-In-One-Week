#include "Maths.h"

float Maths::Cubic(const float& a, const float& b, const float& c, const float& d, const float& factor) {
	return b + 0.5f * factor * (c - a + factor * (2.f * a - 5.f * b + 4.f * c - d + factor * (3.f * (b - c) + d - a)));
}

float Maths::Map(const float& v, const float& fromMin, const float& fromMax, const float& toMin, const float& toMax, const bool& clamp) {
	float factor = (v - fromMin) / (fromMax - fromMin);
	if (clamp) return std::clamp(std::lerp(toMin, toMax, factor), toMin, toMax);
	return std::lerp(toMin, toMax, factor);
}
