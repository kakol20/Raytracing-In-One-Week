#pragma once

#include <algorithm>
#include <cmath>

namespace Maths {
	float Cubic(const float& a, const float& b, const float& c, const float& d, const float& factor);

	float Map(const float& v, const float& fromMin, const float& fromMax, const float& toMin, const float& toMax, const bool& clamp);
};