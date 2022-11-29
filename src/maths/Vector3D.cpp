#include <cmath>

#include "../utility/Random.h"
#include "Maths.h"

#include "Vector3D.h"

Vector3D::Vector3D(const double& x, const double& y, const double& z) {
	m_x = x;
	m_y = y;
	m_z = z;
}

Vector3D& Vector3D::operator=(const Vector3D& other) {
	if (this == &other) return *this;

	m_x = other.m_x;
	m_y = other.m_y;
	m_z = other.m_z;

	return *this;
}

Vector3D& Vector3D::operator/=(const Vector3D& other) {
	m_x /= other.m_x;
	m_y /= other.m_y;
	m_z /= other.m_z;

	return *this;
}

Vector3D& Vector3D::operator*=(const Vector3D& other) {
	m_x *= other.m_x;
	m_y *= other.m_y;
	m_z *= other.m_z;

	return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& other) {
	m_x -= other.m_x;
	m_y -= other.m_y;
	m_z -= other.m_z;

	return *this;
}

Vector3D& Vector3D::operator+=(const Vector3D& other) {
	m_x += other.m_x;
	m_y += other.m_y;
	m_z += other.m_z;

	return *this;
}

Vector3D& Vector3D::operator/=(const double& scalar) {
	m_x /= scalar;
	m_y /= scalar;
	m_z /= scalar;

	return *this;
}

Vector3D& Vector3D::operator*=(const double& scalar) {
	m_x *= scalar;
	m_y *= scalar;
	m_z *= scalar;

	return *this;
}

double Vector3D::DotProduct(const Vector3D& v) const {
	return (m_x * v.m_x) + (m_y * v.m_y) + (m_z * v.m_z);
}

Vector3D Vector3D::CrossProduct(const Vector3D& v) const {
	return Vector3D(m_y * v.m_z - m_z * v.m_y,
		m_z * v.m_x - m_x * v.m_z,
		m_x * v.m_y - m_y * v.m_x);
}

double Vector3D::SqrToroidalDistance(const Vector3D& a, const Vector3D& b, const Vector3D& min, const Vector3D& max) {
	Vector3D delta = b - a;
	delta.Abs();

	Vector3D minMaxDelta = max - min;
	minMaxDelta.Abs();

	Vector3D mid = minMaxDelta / 2.;

	if (delta.m_x > mid.m_x) delta.m_x = minMaxDelta.m_x - delta.m_x;
	if (delta.m_y > mid.m_y) delta.m_y = minMaxDelta.m_y - delta.m_y;
	if (delta.m_z > mid.m_z) delta.m_z = minMaxDelta.m_z - delta.m_z;

	return std::fabs(delta.SqrMagnitude());
}

void Vector3D::Abs() {
	m_x = std::fabs(m_x);
	m_y = std::fabs(m_y);
	m_z = std::fabs(m_z);
}

void Vector3D::Clamp(const Vector3D& min, const Vector3D& max) {
	m_x = std::clamp(m_x, min.m_x, max.m_x);
	m_y = std::clamp(m_y, min.m_y, max.m_y);
	m_z = std::clamp(m_z, min.m_z, max.m_z);
}

Vector3D Vector3D::Lerp(const Vector3D& min, const Vector3D& max, const double& factor, const bool& clamp) {
	return Vector3D(Maths::Map(factor, 0., 1., min.m_x, max.m_x, clamp),
		Maths::Map(factor, 0., 1., min.m_y, max.m_y, clamp),
		Maths::Map(factor, 0., 1., min.m_z, max.m_z, clamp));
}

double Vector3D::SqrMagnitude() const {
	return (m_x * m_x) + (m_y * m_y) + (m_z * m_z);
}

double Vector3D::Magnitude() const {
	return std::sqrt(SqrMagnitude());
}

void Vector3D::Normalize() {
	double mag = SqrMagnitude();
	if (mag != 1.) {
		mag = std::sqrt(mag);

		m_x /= mag;
		m_y /= mag;
		m_z /= mag;
	}
}

Vector3D Vector3D::Reflect(const Vector3D& vector, const Vector3D& normal) {
	Vector3D t = Vector3D::One * normal.DotProduct(vector);
	t *= normal * 2.;
	t = vector - t;
	t.Normalize();
	return t;
}

Vector3D Vector3D::Refract(const Vector3D& vector, const Vector3D& normal, const double refractionRatio) {
	Vector3D vectorInv = vector * -1.;

	double cosTheta = std::fmin(1., vectorInv.DotProduct(normal));

	Vector3D rOutPerp = (vector + (normal * cosTheta)) * refractionRatio;
	Vector3D rOutPara = normal * -std::sqrt(std::fabs(1. - rOutPerp.SqrMagnitude()));

	double sinTheta = std::sqrt(1. - cosTheta * cosTheta);

	bool cannotRefract = refractionRatio * sinTheta > 1.;

	if (cannotRefract) {
		return Vector3D::Reflect(vector, normal);
	} else {
		Vector3D refract = rOutPerp + rOutPara;
		refract.Normalize();
		return refract;
	}
}

Vector3D Vector3D::UVSphere(const Vector3D& v) {
	return Vector3D(0.5f + (std::atan2(v.m_x, v.m_z) / Maths::TAU), 0.5f - (std::asin(v.m_y) / Maths::PI), 0.);
}

Vector3D Vector3D::RandomVector(const double& min, const double& max) {
	return Vector3D(Random::RandomDouble(min, max), Random::RandomDouble(min, max), Random::RandomDouble(min, max));
}

Vector3D Vector3D::RandomInHemisphere(const Vector3D& normal) {
	Vector3D o = Vector3D::RandomInUnitSphere();

	if (normal.DotProduct(o) < 0.) o *= -1.;
	return o;
}

Vector3D Vector3D::RandomInUnitDisk() {
	Vector3D o(Random::RandomDouble(-1., 1.), Random::RandomDouble(-1., 1.));

	if (o.SqrMagnitude() > 1.) o.Normalize();
	return o;
}

Vector3D Vector3D::RandomInUnitSphere() {
	Vector3D o = Vector3D::RandomVector(-1., 1.);

	if (o.SqrMagnitude() > 1.) o.Normalize();
	return o;
}

Vector3D Vector3D::RandomUnitVector() {
	Vector3D o = Vector3D::RandomVector(-1., 1.);
	o.Normalize();
	return o;
}
