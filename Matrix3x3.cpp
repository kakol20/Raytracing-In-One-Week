#include "Matrix3x3.h"

Matrix3x3::Matrix3x3(const Vector3D a, const Vector3D b, const Vector3D c) {
	m_r0 = a;
	m_r1 = b;
	m_r2 = c;
}

Vector3D Matrix3x3::operator*(const Vector3D& v) const {
	return Vector3D(Vector3D::DotProduct(m_r0, v), Vector3D::DotProduct(m_r1, v), Vector3D::DotProduct(m_r2, v));
}

float Matrix3x3::Determinant() {
	float det1 = m_r0.GetX() * ((m_r1.GetY() * m_r2.GetZ()) - (m_r1.GetZ() * m_r2.GetY()));
	float det2 = m_r1.GetX() * ((m_r0.GetY() * m_r2.GetZ()) - (m_r0.GetZ() * m_r2.GetY()));
	float det3 = m_r2.GetX() * ((m_r0.GetY() * m_r1.GetZ()) - (m_r0.GetZ() * m_r1.GetY()));
	return det1 - det2 + det3;
}

void Matrix3x3::Normalize() {
	float determinant = Determinant();

	m_r0 /= determinant;
	m_r1 /= determinant;
	m_r2 /= determinant;
}
