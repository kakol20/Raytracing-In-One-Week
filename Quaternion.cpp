#include <cmath>

#include "Quaternion.h"

Quaternion::Quaternion(const float w, const float i, const float j, const float k) {
	m_w = w;
	m_i = i;
	m_j = j;
	m_k = k;
}

Quaternion::~Quaternion() {
}

Quaternion& Quaternion::operator=(const Quaternion& copyQuaternion) {
	if (this == &copyQuaternion) return *this;

	m_w = copyQuaternion.m_w;
	m_i = copyQuaternion.m_i;
	m_j = copyQuaternion.m_j;
	m_k = copyQuaternion.m_k;

	return *this;
}
Quaternion Quaternion::HamProduct(const Quaternion& q1, const Quaternion& q2) {
	float w = (q1.m_w * q2.m_w) - (q1.m_i * q2.m_i) - (q1.m_j * q2.m_j) - (q1.m_k * q2.m_k);
	float i = (q1.m_w * q2.m_i) + (q1.m_i * q2.m_w) + (q1.m_j * q2.m_k) - (q1.m_k * q2.m_j);
	float j = (q1.m_w * q2.m_j) - (q1.m_i * q2.m_k) + (q1.m_j * q2.m_w) + (q1.m_k * q2.m_i);
	float k = (q1.m_w * q2.m_k) + (q1.m_i * q2.m_j) - (q1.m_j * q2.m_i) + (q1.m_k * q2.m_w);

	Quaternion out = Quaternion(w, i, j, k);
	//out.Normalize();
	return out;
}

Quaternion Quaternion::RotationQuat(const Quaternion& rotation, const Quaternion& point) {
	Quaternion rInv = rotation;
	rInv.Conjugate();

	return Quaternion::HamProduct(Quaternion::HamProduct(rotation, point), rInv);
}

Quaternion Quaternion::ToQuaternionYXZ(const float y, const float x, const float z) {
	float sin1 = sin(y / 2.f);
	float cos1 = cos(y / 2.f);

	float sin2 = sin(x / 2.f);
	float cos2 = cos(x / 2.f);

	float sin3 = sin(z / 2.f);
	float cos3 = cos(z / 2.f);

	float q1 = (sin1 * sin2 * sin3) + (cos1 * cos2 * cos3);
	float q2 = (sin1 * sin3 * cos1) + (sin2 * cos1 * cos3);
	float q3 = (sin1 * cos2 * cos3) - (sin2 * sin3 * cos1);
	float q4 = -(sin1 * sin2 * cos3) + (sin3 * cos1 * cos2);

	Quaternion out = Quaternion(q1, q2, q3, q4);
	//out.Normalize();
	return out;
}

Quaternion Quaternion::ToQuaternionZXY(const float z, const float x, const float y) {
	float sin1 = sin(z / 2.f);
	float cos1 = cos(z / 2.f);

	float sin2 = sin(x / 2.f);
	float cos2 = cos(x / 2.f);

	float sin3 = sin(y / 2.f);
	float cos3 = cos(y / 2.f);

	float q1 = -(sin1 * sin2 * sin3) + (cos1 * cos2 * cos3);
	float q2 = -(sin1 * sin3 * cos2) + (sin2 * cos1 * cos3);
	float q3 = (sin1 * sin2 * cos3) + (sin3 * cos1 * cos2);
	float q4 = (sin1 * cos2 * cos3) + (sin2 * sin3 * cos1);

	Quaternion out = Quaternion(q1, q2, q3, q4);
	//out.Normalize();
	return out;
}

void Quaternion::Conjugate() {
	m_i *= -1.f;
	m_j *= -1.f;
	m_k *= -1.f;
}

void Quaternion::Normalize() {
	float sqrMag = (m_w * m_w) + (m_i * m_i) + (m_j * m_j) + (m_k * m_k);
	float mag = sqrt(sqrMag);

	m_w /= mag;
	m_i /= mag;
	m_j /= mag;
	m_k /= mag;
}

Vector3D Quaternion::GetIJK() {
	return Vector3D(m_i, m_j, m_k);
}