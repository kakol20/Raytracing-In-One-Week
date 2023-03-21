#include "Quaternion.h"

Quaternion::Quaternion(const Float w, const Float i, const Float j, const Float k) {
	m_w = w;
	m_i = i;
	m_j = j;
	m_k = k;
}

Quaternion::Quaternion(const Quaternion& other) {
	m_i = other.m_i;
	m_j = other.m_j;
	m_k = other.m_k;
	m_w = other.m_w;
}

Quaternion::Quaternion(const Vector3D& vec) {
	m_w = 0;
	m_i = vec.GetX();
	m_j = vec.GetY();
	m_k = vec.GetZ();
}

void Quaternion::AxisRotation(const Vector3D& axis, const Float radians) {
	Float sinTheta = Float::Sin(radians / 2);
	Float cosTheta = Float::Cos(radians / 2);

	/*Vector3D ijk = axis;
	ijk *= sinTheta;*/

	m_w = cosTheta;
	m_i = axis.GetX() * sinTheta;
	m_j = axis.GetY() * sinTheta;
	m_k = axis.GetZ() * sinTheta;

	Normalize();
}

Quaternion Quaternion::ShortestArc(const Vector3D& v1, const Vector3D& v2) {
	Quaternion q = Vector3D::CrossProduct(v1, v2);
	q.m_w = Float::Sqrt(Float::Pow(v1.Magnitude(), 2) * Float::Pow(v2.Magnitude(), 2)) + Vector3D::DotProduct(v1, v2);

	q.Normalize();

	return q;
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

Vector3D Quaternion::RotateVector(const Vector3D& vec, const bool normalize) const{
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
	} else {
		os << " + ";
	}
	os << l_i << "i";

	if (num.m_j < 0) {
		os << " - ";
	} else {
		os << " + ";
	}
	os << l_j << "j";

	if (num.m_k < 0) {
		os << " - ";
	} else {
		os << " + ";
	}
	os << l_k << "k";

	return os;
}