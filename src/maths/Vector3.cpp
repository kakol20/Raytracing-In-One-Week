#include "Maths.h"

#include "Vector3.h"

sf::Vector3f& Vector3::operator*=(sf::Vector3f& a, const sf::Vector3f& b) {
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	return a;
}

sf::Vector3f& Vector3::operator/=(sf::Vector3f& a, const sf::Vector3f& b) {
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	return a;
}

sf::Vector3f& Vector3::operator+=(sf::Vector3f& a, const sf::Vector3f& b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

sf::Vector3f& Vector3::operator-=(sf::Vector3f& a, const sf::Vector3f& b) {
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

sf::Vector3f Vector3::operator*(const sf::Vector3f& a, const sf::Vector3f& b) {
	return sf::Vector3f(
		a.x * b.x,
		a.y * b.y,
		a.z * b.z
	);
}

sf::Vector3f Vector3::operator/(const sf::Vector3f& a, const sf::Vector3f& b) {
	return sf::Vector3f(
		a.x / b.x,
		a.y / b.y,
		a.z / b.z
	);
}

sf::Vector3f Vector3::operator+(const sf::Vector3f& a, const sf::Vector3f& b) {
	return sf::Vector3f(
		a.x + b.x,
		a.y + b.y,
		a.z + b.z
	);
}

sf::Vector3f Vector3::operator-(const sf::Vector3f& a, const sf::Vector3f& b) {
	return sf::Vector3f(
		a.x - b.x,
		a.y - b.y,
		a.z - b.z
	);
}

sf::Vector3f Vector3::operator*(const sf::Vector3f& a, const float& b) {
	return sf::Vector3f(a.x * b, a.y * b, a.z * b);
}

sf::Vector3f Vector3::operator/(const sf::Vector3f& a, const float& b) {
	return sf::Vector3f(a.x / b, a.y / b, a.z / b);
}

sf::Vector3f Vector3::operator-(const sf::Vector3f& a) {
	return a * -1.f;
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

sf::Vector3f Vector3::Clamp(const sf::Vector3f& val, const sf::Vector3f& min, const sf::Vector3f& max) {
	return sf::Vector3f(std::clamp(val.x, min.x, max.x), std::clamp(val.y, min.y, max.y), std::clamp(val.z, min.z, max.z));
}

sf::Vector3f Vector3::Lerp(const sf::Vector3f& min, const sf::Vector3f& max, const float& factor, const bool& clamp) {
	float x = std::lerp(min.x, max.x, factor);
	float y = std::lerp(min.y, max.y, factor);
	float z = std::lerp(min.z, max.z, factor);

	x = clamp ? std::clamp(x, min.x, max.x) : x;
	y = clamp ? std::clamp(y, min.y, max.y) : y;
	z = clamp ? std::clamp(z, min.z, max.z) : z;
	return sf::Vector3f(x, y, z);
}

float Vector3::SqrMagnitude(const sf::Vector3f& v) {
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

float Vector3::Magnitude(const sf::Vector3f& v) {
	float sqrMag = Vector3::SqrMagnitude(v);
	return sqrMag == 1.f ? 1.f : std::sqrtf(sqrMag);
}

sf::Vector3f Vector3::Normalize(const sf::Vector3f& v) {
	float magnitude = Vector3::Magnitude(v);
	return v / magnitude;
}

sf::Vector3f Vector3::Reflect(const sf::Vector3f& vector, const sf::Vector3f& normal) {
	sf::Vector3f t = sf::Vector3f(1.f, 1.f, 1.f) * Vector3::DotProduct(normal, vector);
	t *= normal * 2.f;
	t = vector - t;
	return Vector3::Normalize(t);
}

sf::Vector3f Vector3::Refract(const sf::Vector3f& vector, const sf::Vector3f& normal, const float refractionRatio) {
	sf::Vector3f vectorInv = -vector;

	float cosTheta = std::fminf(1.f, Vector3::DotProduct(vectorInv, normal));

	sf::Vector3f rOutPerp = (vector + (normal * cosTheta)) * refractionRatio;
	sf::Vector3f rOutPara = normal * -std::sqrtf(std::abs(1 - Vector3::SqrMagnitude(rOutPerp)));

	float sinTheta = std::sqrtf(1 - cosTheta * cosTheta);

	bool cannotRefract = refractionRatio * sinTheta > 1.f;

	sf::Vector3f refract = rOutPerp + rOutPara;
	if (!cannotRefract) refract = Vector3::Normalize(refract);

	return cannotRefract ? Vector3::Reflect(vector, normal) : refract;
}

sf::Vector3f Vector3::UVSphere(const sf::Vector3f& v) {
	return sf::Vector3f(0.5f + (std::atan2f(v.x, v.z) / Maths::TAU), 0.5f - (std::asinf(v.y) / Maths::PI), 0.f);
}
