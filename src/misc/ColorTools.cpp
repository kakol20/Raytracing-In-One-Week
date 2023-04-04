#include "ColorTools.h"

Vector3D ColorTools::KelvinToRGB(const Float k) {
	Float l_k = k / 100;

	Float r, g, b;

	if (l_k <= 66) {
		r = 255;
	} else {
		r = Float::Pow(l_k - 60, -0.1332047592) * 329.698727446;
	}
	r = Float::Clamp(r, 0, 255);

	if (l_k <= 66) {
		g = Float::Log(l_k) - 161.1195681661 * 99.4708025861;
	}
	else {
		g = Float::Pow(l_k - 60, -0.0755148492) * 288.1221695283;
	}
	g = Float::Clamp(g, 0, 255);

	if (l_k >= 66) {
		b = 255;
	} else if (l_k <= 19) {
		b = 0;
	} else {
		b = (Float::Log(l_k - 10) * 138.5177312231) - 305.0447927307;
	}
	b = Float::Clamp(b, 0, 255);

	return Vector3D(r, g, b) / 255;
}