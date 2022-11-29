#pragma once

#include "../../ext/olcPixelGameEngine.h"

/// <summary>
/// Extend olc::Pixel for arithmetic calculations
/// <para>Treat RGB values as 0-1 values but store as 0-255</para>
/// </summary>
class E_Pixel {
public:
	E_Pixel();
	E_Pixel(const double& r, const double& g, const double& b);
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

	E_Pixel& operator/=(const double& scalar);
	E_Pixel& operator*=(const double& scalar);


	// ----- ARITHMETIC OPERATOR -----

	inline E_Pixel operator/(const E_Pixel& other) const { E_Pixel out(m_r, m_g, m_b); out /= other; return out; };
	inline E_Pixel operator*(const E_Pixel& other) const { E_Pixel out(m_r, m_g, m_b); out *= other; return out; };
	inline E_Pixel operator-(const E_Pixel& other) const { E_Pixel out(m_r, m_g, m_b); out -= other; return out; };
	inline E_Pixel operator+(const E_Pixel& other) const { E_Pixel out(m_r, m_g, m_b); out += other; return out; };

	inline E_Pixel operator/(const double& scalar) const { E_Pixel out(m_r, m_g, m_b); out /= scalar; return out; };
	inline E_Pixel operator*(const double& scalar) const { E_Pixel out(m_r, m_g, m_b); out *= scalar; return out; };

	// ----- OTHER -----

		/// <summary>
		/// Clamps color values between 0 &#38; 255
		/// </summary>
	void Clamp();


	inline void Abs() {
		m_r = std::fabs(m_r);
		m_g = std::fabs(m_g);
		m_b = std::fabs(m_b);
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

	// ----- INTERPOLATION -----

	static E_Pixel Cubic(const E_Pixel& a, const E_Pixel& b, const E_Pixel& c, const E_Pixel& d, const double& factor);
	static E_Pixel Linear(const E_Pixel& a, const E_Pixel& b, const double& factor);

private:
	double m_r, m_g, m_b;

	static const uint8_t Threshold[];
};