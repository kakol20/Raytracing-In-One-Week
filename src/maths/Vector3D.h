#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "../wrapper/Float.h"

class Vector3D {
public:
	Vector3D(const Float& scalar = 0, const bool& includeZAxis = true);
	Vector3D(const Float& x, const Float& y);
	Vector3D(const Float& x, const Float& y, const Float& z);
	~Vector3D() {};

	// ----- ASSIGNMENT OPERATORS -----

	Vector3D& operator*=(const Vector3D& otherVector);
	Vector3D& operator/=(const Vector3D& otherVector);
	Vector3D& operator+=(const Vector3D& otherVector);
	Vector3D& operator=(const Vector3D& otherVector);
	Vector3D& operator-=(const Vector3D& otherVector);

	Vector3D& operator*=(const Float& scalar);
	Vector3D& operator/=(const Float& scalar);
	Vector3D& operator=(const Float& scalar);

	// ----- ARITHMETIC OPERATOR -----

	Vector3D operator-(const Vector3D& otherVector) const;
	Vector3D operator*(const Vector3D& otherVector) const;
	Vector3D operator/(const Vector3D& otherVector) const;
	Vector3D operator+(const Vector3D& otherVector) const;

	Vector3D operator*(const Float& scalar) const;
	Vector3D operator/(const Float& scalar) const;

	/// <summary>
	/// Shorthand for multiplying by -1
	/// </summary>
	/// <returns></returns>
	Vector3D operator-() const;

	// ----- GETTERS -----

	Float GetX() const { return m_x; };
	Float GetY() const { return m_y; };
	Float GetZ() const { return m_z; };

	// ----- OTHER -----

	static Float DotProduct(const Vector3D& v1, const Vector3D& v2);
	static Float ToroidalDistance(const Vector3D& a, const Vector3D& b, const Vector3D& min, const Vector3D& max);
	static Vector3D Abs(const Vector3D& v);
	static Vector3D Clamp(const Vector3D& val, const Vector3D& min, const Vector3D& max);
	static Vector3D CrossProduct(const Vector3D& v1, const Vector3D& v2);

	static Vector3D Reflect(const Vector3D& vector, const Vector3D& normal);
	static Vector3D Refract(const Vector3D& vector, const Vector3D& normal, const Float& refractionRatio);

	// -- Random --

	static Vector3D RandomInHemisphere(const Vector3D& normal);
	static Vector3D RandomInUnitDisk();
	static Vector3D RandomInUnitSphere();
	static Vector3D RandomUnitVector();
	static Vector3D RandomVector(const Float& min = 0, const Float& max = 1, bool includeZAxis = true);

	bool NearZero() const;
	Float Magnitude() const;
	Float SqrMagnitude() const;
	Vector3D UVSphere() const;
	void Normalize();

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
