#pragma once

class Vector3D {
public:
	Vector3D();
	Vector3D(const float x, const float y, const float z);
	Vector3D(const Vector3D& copyVector3d);

	Vector3D& operator=(const Vector3D& copyVector);

	// Division
	const Vector3D operator/(const Vector3D& copyVector) const;
	Vector3D& operator/=(const Vector3D& copyVector);
	const Vector3D operator/(const float scalar) const;
	Vector3D& operator/=(const float scalar);

	// Multiplication
	const Vector3D operator*(const Vector3D& copyVector) const;
	Vector3D& operator*=(const Vector3D& copyVector);
	const Vector3D operator*(const float scalar) const;
	Vector3D& operator*=(const float scalar);

	// Addition
	const Vector3D operator+(const Vector3D& copyVector) const;
	Vector3D& operator+=(const Vector3D& copyVector);

	// Subtraction
	const Vector3D operator-(const Vector3D& copyVector) const;
	Vector3D& operator-=(const Vector3D& copyVector);

	// Length
	const float Magnitude();
	const float SqrMagnitude();

	// Other
	bool NearZero();
	float DotProduct(const Vector3D& copyVector);
	Vector3D CrossProduct(const Vector3D& copyVector);
	Vector3D UnitVector();

	// Random
	static Vector3D Random(const unsigned int bitCount);
	static Vector3D Random(const float min, const float max, const unsigned int bitCount);
	static Vector3D RandomInHemisphere(const Vector3D& normal, const unsigned int bitCount);
	static Vector3D RandomInUnitSphere(const unsigned int bitCount);
	static Vector3D RandomUnitVector(const unsigned int bitCount);
	static Vector3D RandomInUnitDisk(const unsigned int bitCount);

	static Vector3D Lerp(const Vector3D& from, const Vector3D& to, const float factor);

	~Vector3D();

public: // getters
	const float GetX() { return m_x; };
	const float GetY() { return m_y; };
	const float GetZ() { return m_z; };

private:
	float m_x;
	float m_y;
	float m_z;
};