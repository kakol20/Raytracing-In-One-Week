#include "Maths.h"

#include "Vector3.h"

sf::Vector3f Vector3::operator*(const sf::Vector3f& a, const float& b) {
	return sf::Vector3f(a.x * b, a.y * b, a.z * b);
}

sf::Vector3f Vector3::operator/(const sf::Vector3f& a, const float& b) {
	return sf::Vector3f(a.x / b, a.y / b, a.z / b);
}

float Vector3::DotProduct(const sf::Vector3f& v1, const sf::Vector3f& v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

sf::Vector3f Vector3::CrossProduct(const sf::Vector3f& v1, const sf::Vector3f& v2) {
	return sf::Vector3f(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}

float Vector3::ToroidalDistance(const sf::Vector3f& a, const sf::Vector3f& b, const sf::Vector3f& min, const sf::Vector3f& max) {
	sf::Vector3f delta = Vector3::Abs(b - a);
	sf::Vector3f minMaxDelta = Vector3::Abs(max - min);
	sf::Vector3f mid = minMaxDelta / 2.f;

	if (delta.x > mid.x) delta.x = minMaxDelta.x - delta.x;
	if (delta.y > mid.y) delta.y = minMaxDelta.y - delta.y;
	if (delta.z > mid.z) delta.z = minMaxDelta.z - delta.z;

	return std::abs(Vector3::SqrMagnitude(delta));
}

sf::Vector3f Vector3::Abs(const sf::Vector3f& v) {
	return sf::Vector3f(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

float Vector3::SqrMagnitude(const sf::Vector3f& v) {
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}
