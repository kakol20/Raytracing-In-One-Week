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
