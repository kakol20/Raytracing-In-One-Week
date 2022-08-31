#pragma once

#include <SFML/Graphics.hpp>

namespace Vector3 {
	// ----- OPERATOR OVERLOADS -----

	sf::Vector3f operator*(const sf::Vector3f& a, const float& b);
	sf::Vector3f operator/(const sf::Vector3f& a, const float& b);

	// ----- FUNCTIONS -----

	float DotProduct(const sf::Vector3f& v1, const sf::Vector3f& v2);
	sf::Vector3f CrossProduct(const sf::Vector3f& v1, const sf::Vector3f& v2);

	float ToroidalDistance(const sf::Vector3f& a, const sf::Vector3f& b, const sf::Vector3f& min, const sf::Vector3f& max);

	sf::Vector3f Abs(const sf::Vector3f& v);

	float SqrMagnitude(const sf::Vector3f& v);
};