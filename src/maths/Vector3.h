#pragma once

#include <SFML/Graphics.hpp>

namespace rt {
	/// <summary>
/// Extra functions for sf::Vector3f
/// </summary>
	namespace Vector3 {
		// ----- CONSTANTS -----

		// ----- OPERATOR OVERLOADS -----

		sf::Vector3f& operator*=(sf::Vector3f& a, const sf::Vector3f& b);
		sf::Vector3f& operator/=(sf::Vector3f& a, const sf::Vector3f& b);
		sf::Vector3f& operator+=(sf::Vector3f& a, const sf::Vector3f& b);
		sf::Vector3f& operator-=(sf::Vector3f& a, const sf::Vector3f& b);

		sf::Vector3f operator*(const sf::Vector3f& a, const sf::Vector3f& b);
		sf::Vector3f operator/(const sf::Vector3f& a, const sf::Vector3f& b);
		sf::Vector3f operator+(const sf::Vector3f& a, const sf::Vector3f& b);
		sf::Vector3f operator-(const sf::Vector3f& a, const sf::Vector3f& b);

		sf::Vector3f operator*(const sf::Vector3f& a, const float& b);
		sf::Vector3f operator/(const sf::Vector3f& a, const float& b);

		sf::Vector3f operator-(const sf::Vector3f& a);

		// ----- FUNCTIONS -----

		float DotProduct(const sf::Vector3f& v1, const sf::Vector3f& v2);
		sf::Vector3f CrossProduct(const sf::Vector3f& v1, const sf::Vector3f& v2);

		float ToroidalDistance(const sf::Vector3f& a, const sf::Vector3f& b, const sf::Vector3f& min, const sf::Vector3f& max);

		sf::Vector3f Abs(const sf::Vector3f& v);
		sf::Vector3f Clamp(const sf::Vector3f& val, const sf::Vector3f& min, const sf::Vector3f& max);
		sf::Vector3f Lerp(const sf::Vector3f& min, const sf::Vector3f& max, const float& factor, const bool& clamp);

		float SqrMagnitude(const sf::Vector3f& v);
		float Magnitude(const sf::Vector3f& v);
		sf::Vector3f Normalize(const sf::Vector3f& v);

		sf::Vector3f Reflect(const sf::Vector3f& vector, const sf::Vector3f& normal);
		sf::Vector3f Refract(const sf::Vector3f& vector, const sf::Vector3f& normal, const float refractionRatio);

		sf::Vector3f UVSphere(const sf::Vector3f& v);
	};
};