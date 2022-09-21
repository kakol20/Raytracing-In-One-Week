#include <cmath>
#include <algorithm>

#include "Color.h"

rt::Color::Color() {
	m_val = { 0.f, 0.f, 0.f };
}

rt::Color::Color(const float& r, const float& g, const float& b) {
	m_val = { r, g, b };
}

rt::Color::Color(const FloatValue& col) {
	m_val = col;
}

rt::Color::Color(const RawValue& col) {
	m_val = { static_cast<float>(col.r), static_cast<float>(col.g), static_cast<float>(col.b) };
}

rt::Color::Color(const sf::Uint8& r, const sf::Uint8& g, const sf::Uint8& b) {
	m_val = { static_cast<float>(r), static_cast<float>(g), static_cast<float>(b) };
}

rt::Color& rt::Color::operator=(const Color& other) {
	if (this == &other) return  *this;

	m_val = other.m_val;

	return *this;
}

rt::Color& rt::Color::operator+=(const Color& other) {
	m_val.r += other.m_val.r;
	m_val.g += other.m_val.g;
	m_val.b += other.m_val.b;
	return *this;
}

rt::Color& rt::Color::operator-=(const Color& other) {
	m_val.r -= other.m_val.r;
	m_val.g -= other.m_val.g;
	m_val.b -= other.m_val.b;
	return *this;
}

rt::Color& rt::Color::operator*=(const Color& other) {
	m_val.r = (m_val.r * other.m_val.r) / 255.f;
	m_val.g = (m_val.g * other.m_val.g) / 255.f;
	m_val.b = (m_val.b * other.m_val.b) / 255.f;
	return *this;
}

rt::Color& rt::Color::operator/=(const Color& other) {
	m_val.r = (255.f * m_val.r) / other.m_val.r;
	m_val.g = (255.f * m_val.g) / other.m_val.g;
	m_val.b = (255.f * m_val.b) / other.m_val.b;
	return *this;
}

rt::Color& rt::Color::operator*=(const float& scalar) {
	m_val.r = m_val.r * scalar;
	m_val.g = m_val.g * scalar;
	m_val.b = m_val.b * scalar;
	return *this;
}

rt::Color& rt::Color::operator/=(const float& scalar) {
	m_val.r = m_val.r / scalar;
	m_val.g = m_val.g / scalar;
	m_val.b = m_val.b / scalar;
	return *this;
}

rt::Color::RawValue rt::Color::GetRawValue() const {
	return { (sf::Uint8)std::roundf(std::clamp(m_val.r, 0.f, 255.f)),
		(sf::Uint8)std::roundf(std::clamp(m_val.g, 0.f, 255.f)),
		(sf::Uint8)std::roundf(std::clamp(m_val.b, 0.f, 255.f)) };
}

sf::Color rt::Color::GetSFColor() const {
	return sf::Color((sf::Uint8)std::roundf(std::clamp(m_val.r, 0.f, 255.f)),
		(sf::Uint8)std::roundf(std::clamp(m_val.g, 0.f, 255.f)),
		(sf::Uint8)std::roundf(std::clamp(m_val.b, 0.f, 255.f)));
}

void rt::Color::Clamp() {
	m_val.r = std::clamp(m_val.r, 0.f, 255.f);
	m_val.g = std::clamp(m_val.g, 0.f, 255.f);
	m_val.b = std::clamp(m_val.b, 0.f, 255.f);
}

void rt::Color::Dither(const int& x, const int& y, const int& factor) {
	const float f_factor = static_cast<float>(factor);

	float threshold = rt::Color::Threshold[(x % 16) + (y % 16) * 16] / 256.f;
	rt::Color octet(1 / f_factor, 1 / f_factor, 1 / f_factor);

	octet *= 255.f;
	octet *= threshold - 0.5f;

	this->operator+=(octet);
	this->operator*=(f_factor);

	rt::Color::Round();
	this->operator/=(f_factor);

	rt::Color::Clamp();
}

void rt::Color::Round() {
	m_val.r = std::roundf(m_val.r / 255.f) * 255.f;
	m_val.g = std::roundf(m_val.g / 255.f) * 255.f;
	m_val.b = std::roundf(m_val.b / 255.f) * 255.f;
}
