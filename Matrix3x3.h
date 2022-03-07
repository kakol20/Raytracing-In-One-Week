#pragma once
#include "Vector3D.h"
class Matrix3x3 {
public:
	Matrix3x3() : Matrix3x3(Vector3D(1.f, 0.f, 0.f), Vector3D(0.f, 1.f, 0.f), Vector3D(0.f, 0.f, 1.f)) {};
	Matrix3x3(const Vector3D a, const Vector3D b, const Vector3D c);
	~Matrix3x3() {};

	// Using only needed functions

	Vector3D operator*(const Vector3D& v) const;
	
	static Vector3D RotateX(const Vector3D& v, const float radians);
	static Vector3D RotateY(const Vector3D& v, const float radians);
	static Vector3D RotateZ(const Vector3D& v, const float radians);

	float Determinant();
	void Normalize();

private:
	Vector3D m_r0;
	Vector3D m_r1;
	Vector3D m_r2;
};
