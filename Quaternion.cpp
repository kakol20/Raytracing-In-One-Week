#include <cmath>

#include "Quaternion.h"

Quaternion::Quaternion(const float r, const float i, const float j, const float k) {
	m_w = r;
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

	return Quaternion(w, i, j, k);
}

Vector3D Quaternion::GetIJK() {
	return Vector3D(m_i, m_j, m_k);
}
