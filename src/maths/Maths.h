#pragma once

#include <algorithm>
#include <cmath>

namespace Maths {
	// ----- CONSTANTS -----

	const float PI = 3.14159265f;
	const float TAU = Maths::PI * 2.f;

	const float ToRadians = Maths::PI / 180.f;

	float Cubic(const float& a, const float& b, const float& c, const float& d, const float& factor);

	float Map(const float& v, const float& fromMin, const float& fromMax, const float& toMin, const float& toMax, const bool& clamp);
};