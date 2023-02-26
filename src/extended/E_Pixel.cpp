#include "../maths/Maths.h"

#include "E_Pixel.h"

E_Pixel::E_Pixel() {
	m_r = 0.;
	m_g = 0.;
	m_b = 0.;
}

E_Pixel::E_Pixel(const double& r, const double& g, const double& b) {
	m_r = r;
	m_g = g;
	m_b = b;
}

E_Pixel::E_Pixel(const olc::Pixel& pixel) {
	m_r = (double)(pixel.r);
	m_g = (double)(pixel.g);
	m_b = (double)(pixel.b);
}

E_Pixel::E_Pixel(const uint8_t& r, const uint8_t& g, const uint8_t& b) {
	m_r = (double)(r);
	m_g = (double)(g);
	m_b = (double)(b);
}

olc::Pixel E_Pixel::GetOLC() const {
	double r = std::clamp(m_r, 0., 255.);
	double g = std::clamp(m_g, 0., 255.);
	double b = std::clamp(m_b, 0., 255.);
	return olc::Pixel((uint8_t)(r), (uint8_t)(g), (uint8_t)(b));
}

E_Pixel& E_Pixel::operator=(const olc::Pixel& other) {
	m_r = (double)(other.r);
	m_g = (double)(other.g);
	m_b = (double)(other.b);
	return *this;
}

E_Pixel& E_Pixel::operator=(const E_Pixel& other) {
	if (this == &other) return *this;

	m_r = other.m_r;
	m_g = other.m_g;
	m_b = other.m_b;
	return *this;
}

E_Pixel& E_Pixel::operator/=(const E_Pixel& other) {
	m_r = (255. * m_r) / other.m_r;
	m_g = (255. * m_g) / other.m_g;
	m_b = (255. * m_b) / other.m_b;
	return *this;
}

E_Pixel& E_Pixel::operator*=(const E_Pixel& other) {
	m_r = (m_r * other.m_r) / 255.;
	m_g = (m_g * other.m_g) / 255.;
	m_b = (m_b * other.m_b) / 255.;
	return *this;
}

E_Pixel& E_Pixel::operator-=(const E_Pixel& other) {
	m_r -= other.m_r;
	m_g -= other.m_g;
	m_b -= other.m_b;
	return *this;
}

E_Pixel& E_Pixel::operator+=(const E_Pixel& other) {
	m_r += other.m_r;
	m_g += other.m_g;
	m_b += other.m_b;
	return *this;
}

E_Pixel& E_Pixel::operator/=(const double& scalar) {
	m_r /= scalar;
	m_g /= scalar;
	m_b /= scalar;
	return *this;
}

E_Pixel& E_Pixel::operator*=(const double& scalar) {
	m_r *= scalar;
	m_g *= scalar;
	m_b *= scalar;
	return *this;
}

void E_Pixel::Clamp() {
	m_r = std::clamp(m_r, 0., 255.);
	m_g = std::clamp(m_g, 0., 255.);
	m_b = std::clamp(m_b, 0., 255.);
}

void E_Pixel::Dither(const int& x, const int& y, const int& factor) {
	const double f_factor = (double)(factor);

	double threshold = (double)(E_Pixel::Threshold[(x % 16) + (y % 16) * 16]) / 256.;
	E_Pixel octet(1. / f_factor, 1. / f_factor, 1. / f_factor);

	octet *= 255.;
	octet *= threshold - 0.5;

	this->operator+=(octet);
	this->operator*=(f_factor);

	E_Pixel::Round();
	this->operator/=(f_factor);

	E_Pixel::Clamp();
}

void E_Pixel::Round() {
	m_r = std::round(m_r / 255.) * 255.;
	m_g = std::round(m_g / 255.) * 255.;
	m_b = std::round(m_b / 255.) * 255.;
}

void E_Pixel::LinearToSRGB() {
	m_r /= 255.;
	m_g /= 255.;
	m_b /= 255.;

	if (m_r <= 0.0031308) {
		m_r *= 12.92;
	}
	else {
		m_r = ((std::pow(m_r, 1. / 2.4)) * 1.055) - 0.055;
	}

	if (m_g <= 0.0031308) {
		m_g *= 12.92;
	}
	else {
		m_g = ((std::pow(m_g, 1. / 2.4)) * 1.055) - 0.055;
	}

	if (m_b <= 0.0031308) {
		m_b *= 12.92;
	}
	else {
		m_b = ((std::pow(m_b, 1. / 2.4)) * 1.055) - 0.055;
	}

	m_r *= 255.;
	m_g *= 255.;
	m_b *= 255.;
}

void E_Pixel::SRGBtoLinear() {
	m_r /= 255.;
	m_g /= 255.;
	m_b /= 255.;

	if (m_r <= 0.04045) {
		m_r /= 12.92;
	}
	else {
		m_r = std::pow((m_r + 0.055) / 1.055, 2.4);
	}

	if (m_g <= 0.04045) {
		m_g /= 12.92;
	}
	else {
		m_g = std::pow((m_g + 0.055) / 1.055, 2.4);
	}

	if (m_b <= 0.04045) {
		m_b /= 12.92;
	}
	else {
		m_b = std::pow((m_b + 0.055) / 1.055, 2.4);
	}

	m_r *= 255.;
	m_g *= 255.;
	m_b *= 255.;
}

E_Pixel E_Pixel::Cubic(const E_Pixel& a, const E_Pixel& b, const E_Pixel& c, const E_Pixel& d, const double& factor) {
	double r = Maths::Cubic(a.m_r, b.m_r, c.m_r, d.m_r, factor);
	double g = Maths::Cubic(a.m_g, b.m_g, c.m_g, d.m_g, factor);
	double c_b = Maths::Cubic(a.m_b, b.m_b, c.m_b, d.m_b, factor);

	return E_Pixel(r, g, c_b);
}

E_Pixel E_Pixel::Linear(const E_Pixel& a, const E_Pixel& b, const double& factor) {
	double r = Maths::Map(factor, 0., 1., a.m_r, b.m_r);
	double g = Maths::Map(factor, 0., 1., a.m_g, b.m_g);
	double c_b = Maths::Map(factor, 0., 1., a.m_b, b.m_b);
	return E_Pixel(r, g, c_b);
}
