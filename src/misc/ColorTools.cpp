#include "ColorTools.h"

Vector3D ColorTools::KelvinToRGB(const Float k) {
	Float temp = k / 100;

	Float r, g, b;

	if (temp <= 66) {
		r = 255;

		g = temp;
		g = (Float::Log(g) * 99.4708025861) - 161.1195681661;
	} else {
		r = temp - 60;
		r = Float::Pow(r, -0.1332047592) * 329.698727446;

		g = temp - 60;
		g = Float::Pow(g, -0.0755148492) * 288.1221695283;
	}

	if (temp >= 66) {
		b = 255;
	} else if (temp <= 19) {
		b = 0;
	} else {
		b = temp - 10;
		b = (Float::Log(b) * 138.5177312231) - 305.0447927307;
	}

	r = Float::Clamp(r, 0, 255);
	g = Float::Clamp(g, 0, 255);
	b = Float::Clamp(b, 0, 255);

	r /= 255;
	g /= 255;
	b /= 255;

	return Vector3D(r, g, b);
}