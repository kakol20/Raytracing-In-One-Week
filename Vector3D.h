#pragma once

class Vector3D {
public:
	Vector3D();
	Vector3D(const float x, const float y, const float z);
	Vector3D(const Vector3D& copyVector3d);

	Vector3D& operator=(const Vector3D& copyVector);

	// Division
	Vector3D& operator/(const Vector3D& copyVector);
	Vector3D& operator/=(const Vector3D& copyVector);
	Vector3D& operator/(const float scalar);
	Vector3D& operator/=(const float scalar);

	// Multiplication
	Vector3D& operator*(const Vector3D& copyVector);
	Vector3D& operator*=(const Vector3D& copyVector);
	Vector3D& operator*(const float scalar);
	Vector3D& operator*=(const float scalar);

	// Addition
	Vector3D& operator+(const Vector3D& copyVector);
	Vector3D& operator+=(const Vector3D& copyVector);

	// Subtraction
	Vector3D& operator-(const Vector3D& copyVector);
	Vector3D& operator-=(const Vector3D& copyVector);

	// Length
	const float Magnitude();
	const float SqrMagnitude();

	// Other
	const float DotProduct(const Vector3D& copyVector);
	void CrossProduct(const Vector3D& copyVector);
	void UnitVector();

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