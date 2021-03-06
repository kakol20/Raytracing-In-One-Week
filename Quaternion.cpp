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
Quaternion Quaternion::operator-() const {
	return Quaternion(m_w, m_i * -1.f, m_j * -1.f, m_k * -1.f);
}
Quaternion Quaternion::HamProduct(const Quaternion& q1, const Quaternion& q2) {
	float w = (q1.m_w * q2.m_w) - (q1.m_i * q2.m_i) - (q1.m_j * q2.m_j) - (q1.m_k * q2.m_k);
	float i = (q1.m_w * q2.m_i) + (q1.m_i * q2.m_w) + (q1.m_j * q2.m_k) - (q1.m_k * q2.m_j);
	float j = (q1.m_w * q2.m_j) - (q1.m_i * q2.m_k) + (q1.m_j * q2.m_w) + (q1.m_k * q2.m_i);
	float k = (q1.m_w * q2.m_k) + (q1.m_i * q2.m_j) - (q1.m_j * q2.m_i) + (q1.m_k * q2.m_w);
	//out.Normalize();
	return Quaternion(w, i, j, k);
}

Quaternion& Quaternion::operator*=(const Quaternion& other) {
	float w = (m_w * other.m_w) - (m_i * other.m_i) - (m_j * other.m_j) - (m_k * other.m_k);
	float i = (m_w * other.m_i) + (m_i * other.m_w) + (m_j * other.m_k) - (m_k * other.m_j);
	float j = (m_w * other.m_j) - (m_i * other.m_k) + (m_j * other.m_w) + (m_k * other.m_i);
	float k = (m_w * other.m_k) + (m_i * other.m_j) - (m_j * other.m_i) + (m_k * other.m_w);

	m_w = w;
	m_i = i;
	m_j = j;
	m_k = k;
	return *this;
}

Quaternion Quaternion::FromTwoPoints(Vector3D& v1, Vector3D& v2) {
	Quaternion q;
	Vector3D a = Vector3D::CrossProduct(v1, v2);
	q = Quaternion(sqrtf(v1.SqrMagnitude() * v2.SqrMagnitude()) + Vector3D::DotProduct(v1, v2), a.GetX(), a.GetY(), a.GetZ());
	q.Normalize();
	return q;
}

Quaternion Quaternion::RotationQuat(const Quaternion& rotation, const Quaternion& point) {
	Quaternion rInv = -rotation;

	return Quaternion::HamProduct(Quaternion::HamProduct(rotation, point), rInv);
}

Quaternion Quaternion::RotationQuat(const Quaternion& rotation, const Vector3D& point) {
	return Quaternion::RotationQuat(rotation, Quaternion::VectorToPure(point));
}

Vector3D Quaternion::RotationVec(const Quaternion& rotation, const Vector3D& v) {
	Quaternion pureV = Quaternion::VectorToPure(v);
	Quaternion result = rotation;
	result *= pureV;
	result *= -rotation;
	return result.GetIJK();
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

Quaternion Quaternion::VectorToPure(const Vector3D& v) {
	Vector3D l_v = v;
	return Quaternion(0.f, l_v.GetX(), l_v.GetY(), l_v.GetZ());
}

Quaternion Quaternion::AxisToRotation(const Vector3D& axis, const float radians) {
	float sinTheta = sin(radians / 2.f);
	float cosTheta = cos(radians / 2.f);
	Vector3D ijk = axis * sinTheta;

	return Quaternion(cosTheta, ijk.GetX(), ijk.GetY(), ijk.GetZ());
}
