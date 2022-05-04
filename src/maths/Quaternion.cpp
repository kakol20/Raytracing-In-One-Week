#include "Quaternion.h"

Quaternion::Quaternion(const Float& w, const Float& i, const Float& j, const Float& k) {
	m_w = w;
	m_i = i;
	m_j = j;
	m_k = k;
}

Quaternion::Quaternion(const Vector3D& vec) {
	m_w = 0;
	m_i = vec.GetX();
	m_j = vec.GetY();
	m_k = vec.GetZ();
}

Quaternion& Quaternion::operator*=(const Quaternion& otherQuaternion) {
	Float w = (m_w * otherQuaternion.m_w) - (m_i * otherQuaternion.m_i) - (m_j * otherQuaternion.m_j) - (m_k * otherQuaternion.m_k);
	Float i = (m_w * otherQuaternion.m_i) + (m_i * otherQuaternion.m_w) + (m_j * otherQuaternion.m_k) - (m_k * otherQuaternion.m_j);
	Float j = (m_w * otherQuaternion.m_j) - (m_i * otherQuaternion.m_k) + (m_j * otherQuaternion.m_w) + (m_k * otherQuaternion.m_i);
	Float k = (m_w * otherQuaternion.m_k) + (m_i * otherQuaternion.m_j) - (m_j * otherQuaternion.m_i) + (m_k * otherQuaternion.m_w);

	m_w = w;
	m_i = i;
	m_j = j;
	m_k = k;
	return *this;
}

Quaternion& Quaternion::operator=(const Quaternion& otherQuaternion) {
	if (this == &otherQuaternion) return *this;
	m_w = otherQuaternion.m_w;
	m_i = otherQuaternion.m_i;
	m_j = otherQuaternion.m_j;
	m_k = otherQuaternion.m_k;
	return *this;
}

Quaternion& Quaternion::operator=(const Vector3D& otherVector) {
	Vector3D copyVector = otherVector;
	m_w = 0;
	m_i = copyVector.GetX();
	m_j = copyVector.GetY();
	m_k = copyVector.GetZ();
	return *this;
}

Quaternion Quaternion::operator*(const Quaternion& otherQuaternion) const {
	Quaternion out(m_w, m_i, m_j, m_k);
	out *= otherQuaternion;
	return out;
}

Vector3D Quaternion::ToVector() {
	return Vector3D(m_i, m_j, m_k);
}

void Quaternion::Conjugate() {
	m_i *= -1;
	m_j *= -1;
	m_k *= -1;
}

void Quaternion::Normalize() {
	Float mag = (m_w * m_w) + (m_i * m_i) + (m_j * m_j) + (m_k * m_k);
	mag = Float::Sqrt(mag);
	m_w /= mag;
	m_i /= mag;
	m_j /= mag;
	m_k /= mag;
}

void Quaternion::AxisRotation(const Vector3D& axis, const Float& radians) {
	Float sinTheta = Float::Sin(radians / 2);
	Float cosTheta = Float::Cos(radians / 2);

	Vector3D ijk = axis * sinTheta;

	m_w = cosTheta;
	m_i = ijk.GetX();
	m_j = ijk.GetY();
	m_k = ijk.GetZ();

	Normalize();
}

void Quaternion::ShortestArc(const Vector3D& v1, const Vector3D& v2) {
	Vector3D a = Vector3D::CrossProduct(v1, v2);

	m_w = Float::Sqrt(v1.SqrMagnitude() * v2.SqrMagnitude()) + Vector3D::DotProduct(v1, v2);
	m_i = a.GetX();
	m_j = a.GetY();
	m_k = a.GetZ();
	Normalize();
}

void Quaternion::XYZRotation(const Vector3D& radians) {
	Quaternion xRotation;
	Quaternion yRotation;
	Quaternion zRotation;

	xRotation.AxisRotation(Vector3D::Right, radians.GetX());
	yRotation.AxisRotation(Vector3D::Up, radians.GetY());
	zRotation.AxisRotation(Vector3D::Forward, radians.GetZ());

	Quaternion rotation = zRotation;
	rotation *= yRotation;
	rotation *= xRotation;

	m_w = rotation.m_w;
	m_i = rotation.m_i;
	m_j = rotation.m_j;
	m_k = rotation.m_k;

	Normalize();
}

void Quaternion::ZYXRotation(const Vector3D& radians) {
	Quaternion xRotation;
	Quaternion yRotation;
	Quaternion zRotation;

	xRotation.AxisRotation(Vector3D::Right, radians.GetX());
	yRotation.AxisRotation(Vector3D::Up, radians.GetY());
	zRotation.AxisRotation(Vector3D::Forward, radians.GetZ());

	Quaternion rotation = xRotation;
	rotation *= yRotation;
	rotation *= zRotation;

	m_w = rotation.m_w;
	m_i = rotation.m_i;
	m_j = rotation.m_j;
	m_k = rotation.m_k;

	Normalize();
}

Vector3D Quaternion::RotateVector(const Vector3D& vec, const bool& normalize) {
	Quaternion quat(m_w, m_i, m_j, m_k);
	Quaternion pureQuat = vec;
	Quaternion quatInv(m_w, m_i, m_j, m_k);
	quatInv.Conjugate();

	quat *= pureQuat;
	quat *= quatInv;

	Vector3D out = quat.ToVector();
	if (normalize) out.Normalize();

	return out;
}

std::ostream& operator<<(std::ostream& os, const Quaternion& num) {
	os << num.m_w;

	Float l_i = Float::Abs(num.m_i);
	Float l_j = Float::Abs(num.m_j);
	Float l_k = Float::Abs(num.m_k);

	if (num.m_i < 0) {
		os << " - ";
	}
	else {
		os << " + ";
	}
	os << l_i << "i";

	if (num.m_j < 0) {
		os << " - ";
	}
	else {
		os << " + ";
	}
	os << l_j << "j";

	if (num.m_k < 0) {
		os << " - ";
	}
	else {
		os << " + ";
	}
	os << l_k << "k";

	return os;
}