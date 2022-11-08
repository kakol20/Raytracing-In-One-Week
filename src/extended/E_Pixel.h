#pragma once

#include "../../ext/olcPixelGameEngine.h"

/// <summary>
/// Extend olc::Pixel for arithmetic calculations
/// <para>Treat RGB values as 0-1 values but store as 0-255</para>
/// </summary>
class E_Pixel {
public:
	E_Pixel();
	E_Pixel(const olc::Pixel& pixel);
	~E_Pixel() {};

private:
	struct FloatVal { float r; float g; float b; };

	FloatVal m_val;
};