#pragma once

#include "../../ext/olcPixelGameEngine.h"

/// <summary>
/// Extend olc::Pixel for arithmetic calculations
/// <para>Treat RGB values as 0-1 values but store as 0-255</para>
/// </summary>
class E_Pixel {
public:
	E_Pixel();
	E_Pixel(const float& r, const float& g, const float& b);
	E_Pixel(const olc::Pixel& pixel);
	E_Pixel(const uint8_t& r, const uint8_t& g, const uint8_t& b);
	~E_Pixel() {};

	olc::Pixel GetOLC() const;

private:
	float m_r, m_g, m_b;
};