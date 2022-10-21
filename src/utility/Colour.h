#pragma once

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

namespace rt {
	class Colour {
	public:
		struct FloatValue { float r; float g; float b; };
		struct RawValue { sf::Uint8 r; sf::Uint8 g; sf::Uint8 b; };

		Colour();
		Colour(const float& r, const float& g, const float& b);
		Colour(const FloatValue& col);
		Colour(const RawValue& col);
		Colour(const sf::Color& col);
		Colour(const sf::Uint8& r, const sf::Uint8& g, const sf::Uint8& b);
		~Colour() {};

		// ----- ASSIGNMENT OPERATORS -----

		rt::Colour& operator=(const rt::Colour& other);
		rt::Colour& operator+=(const rt::Colour& other);
		rt::Colour& operator-=(const rt::Colour& other);
		rt::Colour& operator*=(const rt::Colour& other);
		rt::Colour& operator/=(const rt::Colour& other);

		rt::Colour& operator*=(const float& scalar);
		rt::Colour& operator/=(const float& scalar);

		rt::Colour& operator=(const sf::Vector3f& vec);
		rt::Colour& operator=(const sf::Color& col);

		// ----- ARITHMETIC OPERATORS -----

		inline Colour operator+(const Colour& other) const {
			Colour out(m_val.r, m_val.g, m_val.b);
			out += other;
			return out;
		};
		inline Colour operator-(const Colour& other) const {
			Colour out(m_val.r, m_val.g, m_val.b);
			out -= other;
			return out;
		};
		inline Colour operator*(const Colour& other) const {
			Colour out(m_val.r, m_val.g, m_val.b);
			out *= other;
			return out;
		};
		inline Colour operator/(const Colour& other) const {
			Colour out(m_val.r, m_val.g, m_val.b);
			out /= other;
			return out;
		};

		inline Colour operator*(const float& scalar) const {
			Colour out(m_val.r, m_val.g, m_val.b);
			out *= scalar;
			return out;
		}
		inline Colour operator/(const float& scalar) const {
			Colour out(m_val.r, m_val.g, m_val.b);
			out /= scalar;
			return out;
		}

		// ----- GETTERS & SETTERS -----

		Colour::RawValue GetRawValue() const;
		sf::Color GetSFColour() const;

		inline Colour::FloatValue GetFloatValue() const { return m_val; };

		// ----- OTHER -----

		/// <summary>
		/// Clamps color values between 0 &#38; 255
		/// </summary>
		void Clamp();

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

		static rt::Colour Cubic(const rt::Colour& a, const rt::Colour& b, const rt::Colour& c, const rt::Colour& d, const float& factor);
		static rt::Colour Linear(const rt::Colour& a, const rt::Colour& b, const float& factor);

	private:
		rt::Colour::FloatValue m_val;

		static const unsigned int Threshold[];
	};
};
