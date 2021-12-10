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

	~Vector3D();

private:
	float m_x;
	float m_y;
	float m_z;
};