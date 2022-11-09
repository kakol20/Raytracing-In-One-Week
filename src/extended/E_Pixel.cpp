#include "E_Pixel.h"

E_Pixel::E_Pixel() {
	m_r = 0.f;
	m_g = 0.f;
	m_b = 0.f;
}

E_Pixel::E_Pixel(const float& r, const float& g, const float& b) {
	m_r = r;
	m_g = g;
	m_b = b;
}

E_Pixel::E_Pixel(const olc::Pixel& pixel) {
	m_r = static_cast<float>(pixel.r);
	m_g = static_cast<float>(pixel.g);
	m_b = static_cast<float>(pixel.b);
}

E_Pixel::E_Pixel(const uint8_t& r, const uint8_t& g, const uint8_t& b) {
	m_r = static_cast<float>(r);
	m_g = static_cast<float>(g);
	m_b = static_cast<float>(b);
}

olc::Pixel E_Pixel::GetOLC() const {
	float r = std::clamp(m_r, 0.f, 255.f);
	float g = std::clamp(m_g, 0.f, 255.f);
	float b = std::clamp(m_b, 0.f, 255.f);
	return olc::Pixel(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
}

E_Pixel& E_Pixel::operator=(const olc::Pixel& other) {
	m_r = static_cast<float>(other.r);
	m_g = static_cast<float>(other.g);
	m_b = static_cast<float>(other.b);
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
	m_r = (255.f * m_r) / other.m_r;
	m_g = (255.f * m_g) / other.m_g;
	m_b = (255.f * m_b) / other.m_b;
	return *this;
}

E_Pixel& E_Pixel::operator*=(const E_Pixel& other) {
	m_r = (m_r * other.m_r) / 255.f;
	m_g = (m_g * other.m_g) / 255.f;
	m_b = (m_b * other.m_b) / 255.f;
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

E_Pixel& E_Pixel::operator/=(const float& scalar) {
	m_r /= scalar;
	m_g /= scalar;
	m_b /= scalar;
	return *this;
}

E_Pixel& E_Pixel::operator*=(const float& scalar) {
	m_r *= scalar;
	m_g *= scalar;
	m_b *= scalar;
	return *this;
}

void E_Pixel::Clamp() {
	m_r = std::clamp(m_r, 0.f, 255.f);
	m_g = std::clamp(m_g, 0.f, 255.f);
	m_b = std::clamp(m_b, 0.f, 255.f);
}

void E_Pixel::Dither(const int& x, const int& y, const int& factor) {
	const float f_factor = static_cast<float>(factor);

	float threshold = static_cast<float>(E_Pixel::Threshold[(x % 16) + (y % 16) * 16]) / 256.f;
	E_Pixel octet(1.f / f_factor, 1.f / f_factor, 1.f / f_factor);

	octet *= 255.f;
	octet *= threshold - 0.5f;

	this->operator+=(octet);
	this->operator*=(f_factor);

	E_Pixel::Round();
	this->operator/=(f_factor);

	E_Pixel::Clamp();
}

void E_Pixel::Round() {
	m_r = std::roundf(m_r / 255.f) * 255.f;
	m_g = std::roundf(m_g / 255.f) * 255.f;
	m_b = std::roundf(m_b / 255.f) * 255.f;
}

void E_Pixel::LinearToSRGB() {
	m_r /= 255.f;
	m_g /= 255.f;
	m_b /= 255.f;

	if (m_r <= 0.0031308f) {
		m_r *= 12.92f;
	}
	else {
		m_r = ((std::powf(m_r, 1.f / 2.4f)) * 1.055f) - 0.055f;
	}

	if (m_g <= 0.0031308f) {
		m_g *= 12.92f;
	}
	else {
		m_g = ((std::powf(m_g, 1.f / 2.4f)) * 1.055f) - 0.055f;
	}

	if (m_b <= 0.0031308f) {
		m_b *= 12.92f;
	}
	else {
		m_b = ((std::powf(m_b, 1.f / 2.4f)) * 1.055f) - 0.055f;
	}

	m_r *= 255.f;
	m_g *= 255.f;
	m_b *= 255.f;
}

void E_Pixel::SRGBtoLinear() {
	m_r /= 255.f;
	m_g /= 255.f;
	m_b /= 255.f;

	if (m_r <= 0.04045f) {
		m_r /= 12.92f;
	}
	else {
		m_r = std::powf((m_r + 0.055f) / 1.055f, 2.4f);
	}

	if (m_g <= 0.04045f) {
		m_g /= 12.92f;
	}
	else {
		m_g = std::powf((m_g + 0.055f) / 1.055f, 2.4f);
	}

	if (m_b <= 0.04045f) {
		m_b /= 12.92f;
	}
	else {
		m_b = std::powf((m_b + 0.055f) / 1.055f, 2.4f);
	}

	m_r *= 255.f;
	m_g *= 255.f;
	m_b *= 255.f;
}
