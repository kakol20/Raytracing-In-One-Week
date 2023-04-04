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

Vector3D ColorTools::HSVToRGB(Float H, const Float S, const Float V) {
	H = Float::ModCycled(H, 360);

	const Float C = V * S;
	const Float X = C * (1. - (((H / 60) % 2) - 1));
	const Float m = V - C;

	Float R, G, B;

	if (H < 60) {
		R = C;
		G = X;
	}
	else if (H < 120) {
		R = X;
		G = C;
	}
	else if (H < 180) {
		G = C;
		B = X;
	}
	else if (H < 240) {
		G = X;
		B = C;
	}
	else if (H < 300) {
		R = X;
		B = C;
	}
	else {
		R = C;
		B = X;
	}

	R += m;
	G += m;
	B += m;

	return Vector3D(R, G, B);
}
