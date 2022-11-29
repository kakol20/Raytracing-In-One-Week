#pragma once

class Vector3D {
public:
	static const Vector3D Forward;
	static const Vector3D Up;
	static const Vector3D Right;

	static const Vector3D Zero;
	static const Vector3D One;

public:
	Vector3D(const double& x = 0.f, const double& y = 0.f, const double& z = 0.f);
	~Vector3D() {};

	double GetX() const { return m_x; };
	double GetY() const { return m_y; };
	double GetZ() const { return m_z; };

	// ----- ASSIGNMENT OPERATORS -----

	Vector3D& operator=(const Vector3D& other);
	Vector3D& operator/=(const Vector3D& other);
	Vector3D& operator*=(const Vector3D& other);
	Vector3D& operator-=(const Vector3D& other);
	Vector3D& operator+=(const Vector3D& other);

	Vector3D& operator/=(const double& scalar);
	Vector3D& operator*=(const double& scalar);

	// ----- ARITHMETIC OPERATORS -----

	inline Vector3D operator/(const Vector3D& other) const { Vector3D out(m_x, m_y, m_z); out /= other; return out; };
	inline Vector3D operator*(const Vector3D& other) const { Vector3D out(m_x, m_y, m_z); out *= other; return out; };
	inline Vector3D operator-(const Vector3D& other) const { Vector3D out(m_x, m_y, m_z); out -= other; return out; };
	inline Vector3D operator+(const Vector3D& other) const { Vector3D out(m_x, m_y, m_z); out += other; return out; };

	inline Vector3D operator/(const double& scalar) const { Vector3D out(m_x, m_y, m_z); out /= scalar; return out; };
	inline Vector3D operator*(const double& scalar) const { Vector3D out(m_x, m_y, m_z); out *= scalar; return out; };

	// ----- OTHER -----

	double DotProduct(const Vector3D& v) const;
	Vector3D CrossProduct(const Vector3D& v) const;

	static double SqrToroidalDistance(const Vector3D& a, const Vector3D& b, const Vector3D& min, const Vector3D& max);

	void Abs();
	void Clamp(const Vector3D& min, const Vector3D& max);
	static Vector3D Lerp(const Vector3D& min, const Vector3D& max, const double& factor, const bool& clamp);

	double SqrMagnitude() const;
	double Magnitude() const;
	void Normalize();

	static Vector3D Reflect(const Vector3D& vector, const Vector3D& normal);
	static Vector3D Refract(const Vector3D& vector, const Vector3D& normal, const double refractionRatio);

	static Vector3D UVSphere(const Vector3D& v);

	// ----- RANDOM -----

	static Vector3D RandomVector(const double& min, const double& max);

	static Vector3D RandomInHemisphere(const Vector3D& normal);
	static Vector3D RandomInUnitDisk();
	static Vector3D RandomInUnitSphere();
	static Vector3D RandomUnitVector();

private:
	double m_x, m_y, m_z;
};