#include "Quaternion.h"

Quaternion::Quaternion(const float& w, const float& i, const float& j, const float& k) {
	m_w = w;
	m_i = i;
	m_j = j;
	m_k = k;
}

Quaternion::Quaternion(const sf::Vector3f& vec) {
	m_w = 0.f;
	m_i = vec.x;
	m_j = vec.y;
	m_k = vec.z;
}

void Quaternion::AxisRotation(const sf::Vector3f& axis, const float& radians) {
	float sinTheta = std::sinf(radians / 2);
	float cosTheta = std::cosf(radians / 2);

	/*sf::Vector3f ijk = axis;
	ijk *= sinTheta;*/

	m_w = cosTheta;
	m_i = axis.x * sinTheta;
	m_j = axis.y * sinTheta;
	m_k = axis.z * sinTheta;

	Normalize();
}

void Quaternion::ShortestArc(const sf::Vector3f& v1, const sf::Vector3f& v2) {
	sf::Vector3f a = rt::Vector3::CrossProduct(v1, v2);

	m_w = std::sqrtf(rt::Vector3::SqrMagnitude(v1) * rt::Vector3::SqrMagnitude(v2) + rt::Vector3::DotProduct(v1, v2));
	m_i = a.x;
	m_j = a.y;
	m_k = a.z;
	Normalize();
}

void Quaternion::XYZRotation(const sf::Vector3f& radians) {
	Quaternion xRotation;
	Quaternion yRotation;
	Quaternion zRotation;

	xRotation.AxisRotation(rt::Vector3::Right, radians.x);
	yRotation.AxisRotation(rt::Vector3::Up, radians.y);
	zRotation.AxisRotation(rt::Vector3::Forward, radians.z);

	Quaternion rotation = zRotation;
	rotation *= yRotation;
	rotation *= xRotation;

	m_w = rotation.m_w;
	m_i = rotation.m_i;
	m_j = rotation.m_j;
	m_k = rotation.m_k;

	Normalize();
}

void Quaternion::ZYXRotation(const sf::Vector3f& radians) {
	Quaternion xRotation;
	Quaternion yRotation;
	Quaternion zRotation;

	xRotation.AxisRotation(rt::Vector3::Right, radians.x);
	yRotation.AxisRotation(rt::Vector3::Up, radians.y);
	zRotation.AxisRotation(rt::Vector3::Forward, radians.z);

	Quaternion rotation = xRotation;
	rotation *= yRotation;
	rotation *= zRotation;

	m_w = rotation.m_w;
	m_i = rotation.m_i;
	m_j = rotation.m_j;
	m_k = rotation.m_k;

	Normalize();
}

sf::Vector3f Quaternion::RotateVector(const sf::Vector3f& vec, const bool& normalize) {
	Quaternion quat(m_w, m_i, m_j, m_k);
	Quaternion pureQuat = vec;
	Quaternion quatInv(m_w, m_i, m_j, m_k);
	quatInv.Conjugate();

	quat *= pureQuat;
	quat *= quatInv;

	sf::Vector3f out = quat.ToVector();
	if (normalize) {
		out = rt::Vector3::Normalize(out);
	}

	return out;
}
