#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "../wrapper/Float.h"

class Vector3D {
public:
	Vector3D(const Float scalar = 0, const bool includeZAxis = true);
	//Vector3D(const Float scalar, const bool includeZAxis);
	Vector3D(const Float x, const Float y);
	Vector3D(const Float x, const Float y, const Float z);
	~Vector3D() {};

	// ----- ASSIGNMENT OPERATORS -----

	inline Vector3D& operator*=(const Vector3D& otherVector) {
		m_x *= otherVector.m_x;
		m_y *= otherVector.m_y;
		m_includeZAxis = m_includeZAxis || otherVector.m_includeZAxis;
		m_z = m_includeZAxis ? m_z * otherVector.m_z : 0;
		return *this;
	};
	inline Vector3D& operator/=(const Vector3D& otherVector) {
		m_x /= otherVector.m_x;
		m_y /= otherVector.m_y;
		m_includeZAxis = otherVector.m_includeZAxis;
		m_z = m_includeZAxis ? m_z / otherVector.m_z : 0;
		return *this;
	};
	inline Vector3D& operator+=(const Vector3D& otherVector) {
		m_x += otherVector.m_x;
		m_y += otherVector.m_y;
		m_includeZAxis = m_includeZAxis || otherVector.m_includeZAxis;
		m_z = m_includeZAxis ? m_z + otherVector.m_z : 0;
		return *this;
	};
	inline Vector3D& operator=(const Vector3D& otherVector) {
		if (this == &otherVector) return *this;
		m_x = otherVector.m_x;
		m_y = otherVector.m_y;
		m_includeZAxis = otherVector.m_includeZAxis;
		m_z = m_includeZAxis ? otherVector.m_z : 0;
		return *this;
	};
	inline Vector3D& operator-=(const Vector3D& otherVector) {
		m_x -= otherVector.m_x;
		m_y -= otherVector.m_y;
		m_includeZAxis = m_includeZAxis || otherVector.m_includeZAxis;
		m_z = m_includeZAxis ? m_z - otherVector.m_z : 0;
		return *this;
	};
	inline Vector3D& operator*=(const Float scalar) {
		m_x *= scalar;
		m_y *= scalar;
		m_z = m_includeZAxis ? m_z * scalar : 0;
		return *this;
	};
	inline Vector3D& operator/=(const Float scalar) {
		m_x /= scalar;
		m_y /= scalar;
		m_z = m_includeZAxis ? m_z / scalar : 0;
		return *this;
	};
	inline Vector3D& operator=(const Float scalar) {
		m_x = scalar;
		m_y = scalar;
		m_z = m_includeZAxis ? scalar : 0;
		return *this;
	};

	// ----- ARITHMETIC OPERATOR -----

	inline Vector3D operator-(const Vector3D& otherVector) const {
		return m_includeZAxis || otherVector.m_includeZAxis ?
			Vector3D(m_x - otherVector.m_x, m_y - otherVector.m_y, m_z - otherVector.m_z) :
			Vector3D(m_x - otherVector.m_x, m_y - otherVector.m_y);
	};
	inline Vector3D operator*(const Vector3D& otherVector) const {
		return m_includeZAxis || otherVector.m_includeZAxis ?
			Vector3D(m_x * otherVector.m_x, m_y * otherVector.m_y, m_z * otherVector.m_z) :
			Vector3D(m_x * otherVector.m_x, m_y * otherVector.m_y);
	};
	inline Vector3D operator/(const Vector3D& otherVector) const {
		return otherVector.m_includeZAxis ?
			Vector3D(m_x / otherVector.m_x, m_y / otherVector.m_y, m_z / otherVector.m_z) :
			Vector3D(m_x / otherVector.m_x, m_y / otherVector.m_y);
	};
	inline Vector3D operator+(const Vector3D& otherVector) const {
		return m_includeZAxis || otherVector.m_includeZAxis ?
			Vector3D(m_x + otherVector.m_x, m_y + otherVector.m_y, m_z + otherVector.m_z) :
			Vector3D(m_x + otherVector.m_x, m_y + otherVector.m_y);
	};

	inline Vector3D operator*(const Float scalar) const {
		return m_includeZAxis ?
			Vector3D(m_x * scalar, m_y * scalar, m_z * scalar) :
			Vector3D(m_x * scalar, m_y * scalar);
	};
	inline Vector3D operator/(const Float scalar) const {
		return m_includeZAxis ?
			Vector3D(m_x / scalar, m_y / scalar, m_z / scalar) :
			Vector3D(m_x / scalar, m_y / scalar);
	};

	/// <summary>
	/// Shorthand for multiplying by -1
	/// </summary>
	/// <returns></returns>
	Vector3D operator-() const {
		return m_includeZAxis ?
			Vector3D(m_x * -1, m_y * -1, m_z * -1) :
			Vector3D(m_x * -1, m_y * -1);
	};

	// ----- GETTERS -----

	inline Float GetX() const { return m_x; };
	inline Float GetY() const { return m_y; };
	inline Float GetZ() const { return m_z; };

	// ----- OTHER -----

	static Float DotProduct(const Vector3D& v1, const Vector3D& v2);
	static Float ToroidalDistance(const Vector3D& a, const Vector3D& b, const Vector3D& min, const Vector3D& max);
	static Vector3D Abs(const Vector3D& v);
	static Vector3D Clamp(const Vector3D& val, const Vector3D& min, const Vector3D& max);
	static Vector3D CrossProduct(const Vector3D& v1, const Vector3D& v2);
	static Vector3D Lerp(const Vector3D& min, const Vector3D& max, const Float factor, const bool clamp = false);

	static Vector3D Reflect(const Vector3D& vector, const Vector3D& normal);
	static Vector3D Refract(const Vector3D& vector, const Vector3D& normal, const Float refractionRatio);

	// -- Random --

	static Vector3D RandomInHemisphere(const Vector3D& normal);
	static Vector3D RandomInUnitDisk();
	static Vector3D RandomInUnitSphere();
	static Vector3D RandomUnitVector();
	static Vector3D RandomVector(const Float min = 0, const Float max = 1, bool includeZAxis = true);

	/// <summary>
	/// Random vector using Random::RandomFloatND()
	/// </summary>
	/// <returns>A 3D vector with all axis between -1 and 1</returns>
	static Vector3D RandomVectorND(bool includeZAxis = true);

	inline bool NearZero() const {
		bool xNearZero = Float::Abs(m_x) < Float::NearZero;
		bool yNearZero = Float::Abs(m_y) < Float::NearZero;
		bool zNearZero = Float::Abs(m_z) < Float::NearZero;
		return m_includeZAxis ? (xNearZero && yNearZero && zNearZero) : (xNearZero && yNearZero);
	};
	inline Float Magnitude() const {
		Float sqrMag = SqrMagnitude();
		return sqrMag == 1 ? 1 : Float::Sqrt(sqrMag);
	};
	inline Float SqrMagnitude() const {
		Float addZ = m_includeZAxis ? m_z * m_z : 0;
		return (m_x * m_x) + (m_y * m_y) + addZ;
	};
	inline Vector3D UVSphere() const { return Vector3D(0.5 + (Float::Atan2(m_x, m_z) / Float::TAU), 0.5 - (Float::Asin(m_y) / Float::PI)); };
	inline void Normalize() {
		Float magnitude = Magnitude();
		m_x /= magnitude;
		m_y /= magnitude;
		m_z = m_includeZAxis ? m_z / magnitude : 0;
	};

	// ----- STATIC VARIABLES -----

	static const Vector3D Forward;
	static const Vector3D One;
	static const Vector3D Right;
	static const Vector3D Up;
	static const Vector3D Zero;

private:
	bool m_includeZAxis;
	Float m_x, m_y, m_z;
};

#endif // !VECTOR3D_H
