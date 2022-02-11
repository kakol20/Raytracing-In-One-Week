#include <cmath>

#include "Random.h"

#include "Vector3D.h"
#include <algorithm>

Vector3D::Vector3D() : Vector3D(0.f, 0.f, 0.f) {
}

Vector3D::Vector3D(const float x, const float y, const float z) {
	m_x = x;
	m_y = y;
	m_z = z;
}

Vector3D::Vector3D(const Vector3D& copyVector) : Vector3D(copyVector.m_x, copyVector.m_y, copyVector.m_z) {
}

Vector3D::~Vector3D() {
}

float Vector3D::Magnitude() {
	float sqrtMag = SqrMagnitude();
	if (sqrtMag == 1.f) return 1.f;
	return sqrt(sqrtMag);
}

float Vector3D::SqrMagnitude() {
	return (m_x * m_x) + (m_y * m_y) + (m_z * m_z);
}

void Vector3D::Normalize() {
	float magnitude = Magnitude();

	if (magnitude != 1.f) {
		m_x /= magnitude;
		m_y /= magnitude;
		m_z /= magnitude;
	}
}

float Vector3D::DotProduct(const Vector3D& v1, const Vector3D& v2) {
	return v1.m_x * v2.m_x + v1.m_y * v2.m_y + v1.m_z * v2.m_z;
}

Vector3D Vector3D::CrossProduct(const Vector3D& v1, const Vector3D& v2) {
	return Vector3D(v1.m_y * v2.m_z - v1.m_z * v2.m_y,
		v1.m_z * v2.m_x - v1.m_x * v2.m_z,
		v1.m_x * v2.m_y - v1.m_y * v2.m_x);
}

bool Vector3D::NearZero() {
	const float s = 1e-8f;
	return abs(m_x) < s && abs(m_y) < s && abs(m_z) < s;
}

Vector3D Vector3D::Clamp(const Vector3D& v, const float min, const float max) {
	return Vector3D(std::clamp(v.m_x, min, max), std::clamp(v.m_y, min, max), std::clamp(v.m_z, min, max));
}

Vector3D Vector3D::Lerp(const Vector3D& from, const Vector3D& to, const float factor) {
	//return Vector3D(std::lerp(from.m_x, to.m_x, factor), std::lerp(from.m_y, to.m_y, factor), std::lerp(from.m_z, to.m_z, factor));
	return from + ((to - from) * factor);
}

void Vector3D::UVSphere(float& u, float& v) {
	float PI = 3.141592653f;
	u = 0.5f + (atan2(m_x, m_z) / (2.f * PI));
	v = 0.5f - (asin(m_y) / PI);
}

Vector3D Vector3D::Random(const float min, const float max) {
	return Vector3D(Random::RandFloatRange(min, max), Random::RandFloatRange(min, max), Random::RandFloatRange(min, max));
}

Vector3D Vector3D::RandomInHemisphere(const Vector3D& normal) {
	Vector3D rand = Vector3D::RandomInUnitSphere();
	if (Vector3D::DotProduct(normal, rand) < 0.f) {
		rand *= 1.f;
	}
	return rand;
}

Vector3D Vector3D::RandomInUnitDisk() {
	while (true) {
		Vector3D o = Vector3D(Random::RandFloatRange(-1.f, 1.f), Random::RandFloatRange(-1.f, 1.f), 0.f);

		if (o.SqrMagnitude() >= 1.0) continue;

		return o;
	}
}

Vector3D Vector3D::RandomInUnitSphere() {
	while (true) {
		Vector3D o = Vector3D::Random(-1.f, 1.f);

		if (o.SqrMagnitude() >= 1.0) continue;

		return o;
	}
}

Vector3D Vector3D::RandomUnitVector() {
	Vector3D o = Vector3D::Random(-1.f, 1.f);
	o.Normalize();
	return o;
}

Vector3D& Vector3D::operator=(const Vector3D& copyVector) {
	if (this == &copyVector) return *this;

	m_x = copyVector.m_x;
	m_y = copyVector.m_y;
	m_z = copyVector.m_z;

	return *this;
}

Vector3D Vector3D::operator/(const float scalar) const {
	return Vector3D(m_x / scalar, m_y / scalar, m_z / scalar);
}

Vector3D Vector3D::operator/(const Vector3D& otherVector) const {
	return Vector3D(m_x / otherVector.m_x, m_y / otherVector.m_y, m_z / otherVector.m_z);
}

Vector3D& Vector3D::operator/=(const float scalar) {
	m_x /= scalar;
	m_y /= scalar;
	m_z /= scalar;

	return *this;
	
}

Vector3D& Vector3D::operator/=(const Vector3D& otherVector) {
	m_x /= otherVector.m_x;
	m_y /= otherVector.m_y;
	m_z /= otherVector.m_z;

	return *this;
}

Vector3D Vector3D::operator*(const float scalar) const {
	return Vector3D(m_x * scalar, m_y * scalar, m_z * scalar);
}

Vector3D Vector3D::operator*(const Vector3D& otherVector) const {
	return Vector3D(m_x * otherVector.m_x, m_y * otherVector.m_y, m_z * otherVector.m_z);
}

Vector3D& Vector3D::operator*=(const float scalar) {
	m_x *= scalar;
	m_y *= scalar;
	m_z *= scalar;

	return *this;
}

Vector3D& Vector3D::operator*=(const Vector3D& otherVector) {
	m_x *= otherVector.m_x;
	m_y *= otherVector.m_y;
	m_z *= otherVector.m_z;

	return *this;
}

Vector3D Vector3D::operator+(const Vector3D& otherVector) const {
	return Vector3D(m_x + otherVector.m_x, m_y + otherVector.m_y, m_z + otherVector.m_z);
}

Vector3D Vector3D::operator++(int i) {
	return Vector3D(m_x + i, m_y + i, m_z + i);
}

Vector3D& Vector3D::operator++() {
	m_x++;
	m_y++;
	m_z++;

	return *this;
}

Vector3D& Vector3D::operator+=(const Vector3D& otherVector) {
	m_x += otherVector.m_x;
	m_y += otherVector.m_y;
	m_z += otherVector.m_z;

	return *this;
}

Vector3D Vector3D::operator-(const Vector3D& otherVector) const {
	return Vector3D(m_x - otherVector.m_x, m_y - otherVector.m_y, m_z - otherVector.m_z);
}

Vector3D Vector3D::operator--(int i) {
	return Vector3D(m_x - i, m_y - i, m_z - i);
}

Vector3D& Vector3D::operator--() {
	m_x--;
	m_y--;
	m_z--;

	return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& otherVector) {
	m_x -= otherVector.m_x;
	m_y -= otherVector.m_y;
	m_z -= otherVector.m_z;
	return *this;
}

bool Vector3D::operator==(const Vector3D& otherVector) const {
	return (m_x == otherVector.m_x) && (m_y == otherVector.m_y) && (m_z == otherVector.m_z);
}

bool Vector3D::operator!=(const Vector3D& otherVector) const {
	return !((m_x == otherVector.m_x) && (m_y == otherVector.m_y) && (m_z == otherVector.m_z));
}

Vector3D Vector3D::operator-() const {
	return Vector3D(m_x, m_y, m_z) * -1.f;
}
