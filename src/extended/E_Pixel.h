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

	// ----- ASSIGNMENT OPERATORS -----

	E_Pixel& operator=(const olc::Pixel& other);

	E_Pixel& operator=(const E_Pixel& other);
	E_Pixel& operator/=(const E_Pixel& other);
	E_Pixel& operator*=(const E_Pixel& other);
	E_Pixel& operator-=(const E_Pixel& other);
	E_Pixel& operator+=(const E_Pixel& other);

	E_Pixel& operator/=(const float& scalar);
	E_Pixel& operator*=(const float& scalar);


	// ----- ARITHMETIC OPERATOR -----

	inline E_Pixel operator/(const E_Pixel& other) const { E_Pixel out(m_r, m_g, m_b); out /= other; return out; };
	inline E_Pixel operator*(const E_Pixel& other) const { E_Pixel out(m_r, m_g, m_b); out *= other; return out; };
	inline E_Pixel operator-(const E_Pixel& other) const { E_Pixel out(m_r, m_g, m_b); out -= other; return out; };
	inline E_Pixel operator+(const E_Pixel& other) const { E_Pixel out(m_r, m_g, m_b); out += other; return out; };

	inline E_Pixel operator/(const float& scalar) const { E_Pixel out(m_r, m_g, m_b); out /= scalar; return out; };
	inline E_Pixel operator*(const float& scalar) const { E_Pixel out(m_r, m_g, m_b); out *= scalar; return out; };

	// ----- OTHER -----

		/// <summary>
		/// Clamps color values between 0 &#38; 255
		/// </summary>
	void Clamp();


	inline void Abs() {
		m_r = std::fabsf(m_r);
		m_g = std::fabsf(m_g);
		m_b = std::fabsf(m_b);
	};

	/// <summary>
	/// Uses ordered dithering
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="factor"></param>
	void Dither(const int& x, const int& y, const int& factor = 255);

	/// <summary>
	/// Round color values as if it's scaled down to 0 &#38; 1 (divided by 255)
	/// </summary>
	void Round();

	/// <summary>
	/// Convert Linear space to SRGB space
	/// </summary>
	void LinearToSRGB();

	/// <summary>
	/// Convert SRGB space to Linear space
	/// </summary>
	void SRGBtoLinear();

private:
	float m_r, m_g, m_b;

	static const uint8_t Threshold[];
};