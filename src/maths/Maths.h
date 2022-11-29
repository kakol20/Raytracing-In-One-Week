#pragma once

#include <algorithm>
#include <cmath>

namespace Maths {
	// ----- CONSTANTS -----

	const double PI = 3.1415926535897932384626433832795;
	const double TAU = Maths::PI * 2.f;

	const double ToRadians = Maths::PI / 180.;

	double Cubic(const double& a, const double& b, const double& c, const double& d, const double& factor);

	double Map(const double& v, const double& fromMin, const double& fromMax, const double& toMin, const double& toMax, const bool& clamp = false);
};