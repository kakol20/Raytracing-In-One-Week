#pragma once

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

namespace rt {
	class Color {
	public:
		struct FloatValue { float r; float g; float b; };
		struct RawValue { sf::Uint8 r; sf::Uint8 g; sf::Uint8 b; };

		Color();
		Color(const float& r, const float& g, const float& b);
		Color(const FloatValue& col);
		Color(const RawValue& col);
		Color(const sf::Uint8& r, const sf::Uint8& g, const sf::Uint8& b);
		~Color() {};

		// ----- ASSIGNMENT OPERATORS -----

		Color& operator=(const Color& other);
		Color& operator+=(const Color& other);
		Color& operator-=(const Color& other);
		Color& operator*=(const Color& other);
		Color& operator/=(const Color& other);

		Color& operator*=(const float& scalar);
		Color& operator/=(const float& scalar);

		// ----- ARITHMETIC OPERATORS -----

		inline Color operator+(const Color& other) const {
			Color out(m_val.r, m_val.g, m_val.b);
			out += other;
			return out;
		};
		inline Color operator-(const Color& other) const {
			Color out(m_val.r, m_val.g, m_val.b);
			out -= other;
			return out;
		};
		inline Color operator*(const Color& other) const {
			Color out(m_val.r, m_val.g, m_val.b);
			out *= other;
			return out;
		};
		inline Color operator/(const Color& other) const {
			Color out(m_val.r, m_val.g, m_val.b);
			out /= other;
			return out;
		};

		inline Color operator*(const float& scalar) const {
			Color out(m_val.r, m_val.g, m_val.b);
			out *= scalar;
			return out;
		}
		inline Color operator/(const float& scalar) const {
			Color out(m_val.r, m_val.g, m_val.b);
			out /= scalar;
			return out;
		}

		// ----- GETTERS & SETTERS -----

		Color::RawValue GetRawValue() const;
		sf::Color GetSFColor() const;

		inline Color::FloatValue GetFloatValue() const { return m_val; };

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

	private:
		Color::FloatValue m_val;

		static const unsigned int Threshold[];
	};
};
