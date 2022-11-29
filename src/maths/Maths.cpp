#include "Maths.h"

double Maths::Cubic(const double& a, const double& b, const double& c, const double& d, const double& factor) {
	return b + 0.5 * factor * (c - a + factor * (2. * a - 5. * b + 4. * c - d + factor * (3. * (b - c) + d - a)));
}

double Maths::Map(const double& v, const double& fromMin, const double& fromMax, const double& toMin, const double& toMax, const bool& clamp) {
	double factor = (v - fromMin) / (fromMax - fromMin);
	if (clamp) return std::clamp(std::lerp(toMin, toMax, factor), toMin, toMax);
	return std::lerp(toMin, toMax, factor);
}
