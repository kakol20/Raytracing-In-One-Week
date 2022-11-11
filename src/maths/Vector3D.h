#pragma once

class Vector3D {
public:
	static const Vector3D Forward;
	static const Vector3D Up;
	static const Vector3D Right;

	static const Vector3D Zero;
	static const Vector3D One;

public:
	Vector3D(const float& x = 0.f, const float& y = 0.f, const float& z = 0.f);
	~Vector3D() {};

	float GetX() const { return m_x; };
	float GetY() const { return m_y; };
	float GetZ() const { return m_z; };

	// ----- ASSIGNMENT OPERATORS -----

	Vector3D& operator=(const Vector3D& other);
	Vector3D& operator/=(const Vector3D& other);
	Vector3D& operator*=(const Vector3D& other);
	Vector3D& operator-=(const Vector3D& other);
	Vector3D& operator+=(const Vector3D& other);

	Vector3D& operator/=(const float& scalar);
	Vector3D& operator*=(const float& scalar);

	// ----- ARITHMETIC OPERATORS -----

	inline Vector3D operator/(const Vector3D& other) const { Vector3D out(m_x, m_y, m_z); out /= other; return out; };
	inline Vector3D operator*(const Vector3D& other) const { Vector3D out(m_x, m_y, m_z); out *= other; return out; };
	inline Vector3D operator-(const Vector3D& other) const { Vector3D out(m_x, m_y, m_z); out -= other; return out; };
	inline Vector3D operator+(const Vector3D& other) const { Vector3D out(m_x, m_y, m_z); out += other; return out; };

	inline Vector3D operator/(const float& scalar) const { Vector3D out(m_x, m_y, m_z); out /= scalar; return out; };
	inline Vector3D operator*(const float& scalar) const { Vector3D out(m_x, m_y, m_z); out *= scalar; return out; };

	// ----- OTHER -----

	float DotProduct(const Vector3D& v) const;
	Vector3D CrossProduct(const Vector3D& v) const;

	static float SqrToroidalDistance(const Vector3D& a, const Vector3D& b, const Vector3D& min, const Vector3D& max);

	void Abs();
	void Clamp(const Vector3D& min, const Vector3D& max);
	static Vector3D Lerp(const Vector3D& min, const Vector3D& max, const float& factor, const bool& clamp);

	float SqrMagnitude() const;
	float Magnitude() const;
	void Normalize();

	static Vector3D Reflect(const Vector3D& vector, const Vector3D& normal);
	static Vector3D Refract(const Vector3D& vector, const Vector3D& normal, const float refractionRatio);

	static Vector3D UVSphere(const Vector3D& v);

	// ----- RANDOM -----

	static Vector3D RandomVector(const float& min, const float& max);

	static Vector3D RandomInHemisphere(const Vector3D& normal);
	static Vector3D RandomInUnitDisk();
	static Vector3D RandomInUnitSphere();
	static Vector3D RandomUnitVector();

private:
	float m_x, m_y, m_z;
};