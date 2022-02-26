#include <cmath>

#include "Random.h"

#include "Vector3D.h"
#include <algorithm>

Vector3D::Vector3D() : Vector3D(0.f, 0.f, 0.f) {
}

Vector3D::Vector3D(const float scalar) {
	m_x = scalar;
	m_y = scalar;
	m_z = scalar;
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

	if (magnitude != 1.f && magnitude != 0.f) {
		m_x /= magnitude;
		m_y /= magnitude;
		m_z /= magnitude;
	}
	else if (magnitude == 0.f) {
		m_x = NAN;
		m_y = NAN;
		m_z = NAN;
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
	const float s = 1e-6f;
	return abs(m_x) < s && abs(m_y) < s && abs(m_z) < s;
}

bool Vector3D::Threshold(const float threshold) {
	return abs(m_x) < threshold && abs(m_y) < threshold && abs(m_z) < threshold;
}

void Vector3D::Abs() {
	m_x = abs(m_x);
	m_y = abs(m_y);
	m_z = abs(m_z);
}

bool Vector3D::IsNAN() {
	return isnan(m_x) || isnan(m_y) || isnan(m_z);
}

Vector3D Vector3D::Clamp(const Vector3D& v, const float min, const float max) {
	return Vector3D(std::clamp(v.m_x, min, max), std::clamp(v.m_y, min, max), std::clamp(v.m_z, min, max));
}

Vector3D Vector3D::Lerp(const Vector3D& from, const Vector3D& to, const float factor) {
	//return Vector3D(std::lerp(from.m_x, to.m_x, factor), std::lerp(from.m_y, to.m_y, factor), std::lerp(from.m_z, to.m_z, factor));
	return from + ((to - from) * factor);
}

Vector3D Vector3D::HSVtoRGB(const float h, const float s, const float v) {
	float c = v * s;
	float modh = fmod(h / 60.f, 2.f);
	float x = c * (1.f - abs(modh - 1.f));
	float m = v - c;

	float r = 0.f, g = 0.f, b = 0.f;
	if (h < 60.f) {
		r = c;
		g = x;
	}
	else if (h < 120.f) {
		r = x;
		g = c;
	}
	else if (h < 180.f) {
		g = c;
		b = x;
	}
	else if (h < 240.f) {
		g = x;
		b = c;
	}
	else if (h < 300.f) {
		r = x;
		b = c;
	}
	else {
		r = c;
		b = x;
	}
	return Vector3D(r + m, g + m, b + m);
}

Vector3D Vector3D::KelvinToRGB(const float kelvin) {
	// https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html

	float temperature = kelvin / 100.f;

	float r, g, b;
	if (temperature <= 66.f) {
		r = 255.f;

		g = temperature;
		g = 99.4708025861f * log(g) - 161.1195681661f;
	}
	else {
		r = temperature - 60.f;
		r = 329.698727446f * pow(r, -0.1332047592f);

		g = temperature - 60.f;
		g = 288.1221695283f * pow(g, -0.0755148492f);
	}

	if (temperature >= 66.f) {
		b = 255.f;
	}
	else {
		if (temperature <= 19.f) {
			b = 0.f;
		}
		else {
			b = temperature - 10.f;
			b = 138.5177312231f * log(b) - 305.0447927307f;
		}
	}

	r /= 255.f;
	g /= 255.f;
	b /= 255.f;
	return Vector3D::Clamp(Vector3D(r, g, b), 0.f, 1.f);
}

Vector3D Vector3D::OrderedDithering(const Vector3D col, const int x, const int y, const int factor) {
	const unsigned int Threshold8[] = {
		 0, 32,  8, 40,  2, 34, 10, 42,
		48, 16, 56, 24, 50, 18, 58, 26,
		12, 44,  4, 36, 14, 46,  6, 38,
		60, 28, 52, 20, 62, 30, 54, 22,
		 3, 35, 11, 43,  1, 33,  9, 41,
		51, 19, 59, 27, 49, 17, 57, 25,
		15, 47,  7, 39, 13, 45,  5, 37,
		63, 31, 55, 23, 61, 29, 53, 21
	};

	float r = col.m_x;
	float g = col.m_y;
	float b = col.m_z;

	float threshold = (float)Threshold8[(x % 8) + (y % 8) * 8] / 64.f;
	float octet = 1.f / (float)factor;

	r = r + octet * (threshold - 0.5f);
	r = round(r * (float)factor) / (float)factor;

	g = g + octet * (threshold - 0.5f);
	g = round(g * (float)factor) / (float)factor;

	b = b + octet * (threshold - 0.5f);
	b = round(b * (float)factor) / (float)factor;

	r = std::clamp(r, 0.0f, 1.0f);
	g = std::clamp(g, 0.0f, 1.0f);
	b = std::clamp(b, 0.0f, 1.0f);

	return Vector3D(r, g, b);
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

Vector3D& Vector3D::operator=(const float& scalar) {
	m_x = scalar;
	m_y = scalar;
	m_z = scalar;

	return *this;
}

Vector3D& Vector3D::operator=(const Vector3D& copyVector) {
	if (this == &copyVector) return *this;

	m_x = copyVector.m_x;
	m_y = copyVector.m_y;
	m_z = copyVector.m_z;

	return *this;
}

Vector3D Vector3D::operator/(const float scalar) const {
	if (scalar == 0.f) return Vector3D(1.f);
		
	return Vector3D(m_x / scalar, m_y / scalar, m_z / scalar);
}

Vector3D Vector3D::operator/(const Vector3D& otherVector) const {
	float x = otherVector.m_x == 0.f ? NAN : m_x / otherVector.m_x;
	float y = otherVector.m_y == 0.f ? NAN : m_y / otherVector.m_y;
	float z = otherVector.m_z == 0.f ? NAN : m_z / otherVector.m_z;
	return Vector3D(x, y, z);
}

Vector3D& Vector3D::operator/=(const float scalar) {
	if (scalar == 0.f) {
		m_x = NAN;
		m_y = NAN;
		m_z = NAN;

		return *this;
	}
	
	m_x /= scalar;
	m_y /= scalar;
	m_z /= scalar;

	return *this;
}

Vector3D& Vector3D::operator/=(const Vector3D& otherVector) {
	float x = otherVector.m_x == 0.f ? NAN : m_x / otherVector.m_x;
	float y = otherVector.m_y == 0.f ? NAN : m_y / otherVector.m_y;
	float z = otherVector.m_z == 0.f ? NAN : m_z / otherVector.m_z;

	m_x = x;
	m_y = y;
	m_z = z;

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