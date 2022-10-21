#include <cmath>
#include <algorithm>

#include "../maths/Maths.h"

#include "Colour.h"

rt::Colour::Colour() {
	m_val = { 0.f, 0.f, 0.f };
}

rt::Colour::Colour(const float& r, const float& g, const float& b) {
	m_val = { r, g, b };
}

rt::Colour::Colour(const FloatValue& col) {
	m_val = col;
}

rt::Colour::Colour(const RawValue& col) {
	m_val = { static_cast<float>(col.r), static_cast<float>(col.g), static_cast<float>(col.b) };
}

rt::Colour::Colour(const sf::Color& col) {
	m_val.r = static_cast<float>(col.r);
	m_val.g = static_cast<float>(col.g);
	m_val.b = static_cast<float>(col.b);
}

rt::Colour::Colour(const sf::Uint8& r, const sf::Uint8& g, const sf::Uint8& b) {
	m_val = { static_cast<float>(r), static_cast<float>(g), static_cast<float>(b) };
}

rt::Colour& rt::Colour::operator=(const Colour& other) {
	if (this == &other) return  *this;

	m_val = other.m_val;

	return *this;
}

rt::Colour& rt::Colour::operator+=(const Colour& other) {
	m_val.r += other.m_val.r;
	m_val.g += other.m_val.g;
	m_val.b += other.m_val.b;
	return *this;
}

rt::Colour& rt::Colour::operator-=(const Colour& other) {
	m_val.r -= other.m_val.r;
	m_val.g -= other.m_val.g;
	m_val.b -= other.m_val.b;
	return *this;
}

rt::Colour& rt::Colour::operator*=(const Colour& other) {
	m_val.r = (m_val.r * other.m_val.r) / 255.f;
	m_val.g = (m_val.g * other.m_val.g) / 255.f;
	m_val.b = (m_val.b * other.m_val.b) / 255.f;
	return *this;
}

rt::Colour& rt::Colour::operator/=(const Colour& other) {
	m_val.r = (255.f * m_val.r) / other.m_val.r;
	m_val.g = (255.f * m_val.g) / other.m_val.g;
	m_val.b = (255.f * m_val.b) / other.m_val.b;
	return *this;
}

rt::Colour& rt::Colour::operator*=(const float& scalar) {
	m_val.r = m_val.r * scalar;
	m_val.g = m_val.g * scalar;
	m_val.b = m_val.b * scalar;
	return *this;
}

rt::Colour& rt::Colour::operator/=(const float& scalar) {
	m_val.r = m_val.r / scalar;
	m_val.g = m_val.g / scalar;
	m_val.b = m_val.b / scalar;
	return *this;
}

rt::Colour& rt::Colour::operator=(const sf::Vector3f& vec) {
	m_val.r = vec.x;
	m_val.g = vec.y;
	m_val.b = vec.z;
	return *this;
}

rt::Colour& rt::Colour::operator=(const sf::Color& col) {
	m_val.r = static_cast<float>(col.r);
	m_val.g = static_cast<float>(col.g);
	m_val.b = static_cast<float>(col.b);
	return *this;
}

rt::Colour::RawValue rt::Colour::GetRawValue() const {
	return { (sf::Uint8)std::roundf(std::clamp(m_val.r, 0.f, 255.f)),
		(sf::Uint8)std::roundf(std::clamp(m_val.g, 0.f, 255.f)),
		(sf::Uint8)std::roundf(std::clamp(m_val.b, 0.f, 255.f)) };
}

sf::Color rt::Colour::GetSFColour() const {
	return sf::Color((sf::Uint8)std::roundf(std::clamp(m_val.r, 0.f, 255.f)),
		(sf::Uint8)std::roundf(std::clamp(m_val.g, 0.f, 255.f)),
		(sf::Uint8)std::roundf(std::clamp(m_val.b, 0.f, 255.f)));
}

void rt::Colour::Clamp() {
	m_val.r = std::clamp(m_val.r, 0.f, 255.f);
	m_val.g = std::clamp(m_val.g, 0.f, 255.f);
	m_val.b = std::clamp(m_val.b, 0.f, 255.f);
}

void rt::Colour::Dither(const int& x, const int& y, const int& factor) {
	const float f_factor = static_cast<float>(factor);

	float threshold = rt::Colour::Threshold[(x % 16) + (y % 16) * 16] / 256.f;
	rt::Colour octet(1 / f_factor, 1 / f_factor, 1 / f_factor);

	octet *= 255.f;
	octet *= threshold - 0.5f;

	this->operator+=(octet);
	this->operator*=(f_factor);

	rt::Colour::Round();
	this->operator/=(f_factor);

	rt::Colour::Clamp();
}

void rt::Colour::Round() {
	m_val.r = std::roundf(m_val.r / 255.f) * 255.f;
	m_val.g = std::roundf(m_val.g / 255.f) * 255.f;
	m_val.b = std::roundf(m_val.b / 255.f) * 255.f;
}

void rt::Colour::LinearToSRGB() {
	m_val.r /= 255.f;
	m_val.g /= 255.f;
	m_val.b /= 255.f;

	if (m_val.r <= 0.0031308f) {
		m_val.r *= 12.92f;
	}
	else {
		m_val.r = ((std::powf(m_val.r, 1.f / 2.4f)) * 1.055f) - 0.055f;
	}

	if (m_val.g <= 0.0031308f) {
		m_val.g *= 12.92f;
	}
	else {
		m_val.g = ((std::powf(m_val.g, 1.f / 2.4f)) * 1.055f) - 0.055f;
	}

	if (m_val.b <= 0.0031308f) {
		m_val.b *= 12.92f;
	}
	else {
		m_val.b = ((std::powf(m_val.b, 1.f / 2.4f)) * 1.055f) - 0.055f;
	}

	m_val.r *= 255.f;
	m_val.g *= 255.f;
	m_val.b *= 255.f;
}

void rt::Colour::SRGBtoLinear() {
	m_val.r /= 255.f;
	m_val.g /= 255.f;
	m_val.b /= 255.f;

	if (m_val.r <= 0.04045f) {
		m_val.r /= 12.92f;
	}
	else {
		m_val.r = std::powf((m_val.r + 0.055f) / 1.055f, 2.4f);
	}

	if (m_val.g <= 0.04045f) {
		m_val.g /= 12.92f;
	}
	else {
		m_val.g = std::powf((m_val.g + 0.055f) / 1.055f, 2.4f);
	}

	if (m_val.b <= 0.04045f) {
		m_val.b /= 12.92f;
	}
	else {
		m_val.b = std::powf((m_val.b + 0.055f) / 1.055f, 2.4f);
	}

	m_val.r *= 255.f;
	m_val.g *= 255.f;
	m_val.b *= 255.f;
}

rt::Colour rt::Colour::Cubic(const rt::Colour& a, const rt::Colour& b, const rt::Colour& c, const rt::Colour& d, const float& factor) {
	float r = Maths::Cubic(a.m_val.r, b.m_val.r, c.m_val.r, d.m_val.r, factor);
	float g = Maths::Cubic(a.m_val.g, b.m_val.g, c.m_val.g, d.m_val.g, factor);
	float c_b = Maths::Cubic(a.m_val.b, b.m_val.b, c.m_val.b, d.m_val.b, factor);

	return rt::Colour(r, g, c_b);
}

rt::Colour rt::Colour::Linear(const rt::Colour& a, const rt::Colour& b, const float& factor) {
	float r = Maths::Map(factor, 0.f, 1.f, a.m_val.r, b.m_val.r);
	float g = Maths::Map(factor, 0.f, 1.f, a.m_val.g, b.m_val.g);
	float c_b = Maths::Map(factor, 0.f, 1.f, a.m_val.b, b.m_val.b);
	return rt::Colour(r, g, c_b);
}
