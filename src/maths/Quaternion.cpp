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
#define S_ARC_OLD

#ifdef S_ARC_OLD
	Quaternion q = Vector3D::CrossProduct(v1, v2);
	q.m_w = Float::Sqrt(Float::Pow(v1.Magnitude(), 2) * Float::Pow(v2.Magnitude(), 2)) + Vector3D::DotProduct(v1, v2);
	q.Normalize();
	return q;
#else
	Vector3D v1N = v1;
	v1N.Normalize();

	Vector3D v2N = v2;
	v2N.Normalize();

	Float dotProduct = Vector3D::DotProduct(v1N, v2N);
	Vector3D axis = Vector3D::CrossProduct(v1N, v2N);
	axis.Normalize();

	Float angle = Float::Acos(dotProduct);
	Float halfAngle = angle / 2;

	axis *= Float::Sin(halfAngle);

	Quaternion q = Quaternion(Float::Cos(halfAngle),
		axis.GetX(),
		axis.GetY(),
		axis.GetZ());
	q.Normalize();

	return q;
#endif // S_ARC_OLD
}

void Quaternion::XYZRotation(const Vector3D& radians) {
//#define XYZ_OLD

#ifdef XYZ_OLD
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
#else
	const Float roll = radians.GetX();
	const Float rollSin = Float::Sin(roll / 2);
	const Float rollCos = Float::Cos(roll / 2);

	const Float pitch = radians.GetY();
	const Float pitchSin = Float::Sin(pitch / 2);
	const Float pitchCos = Float::Cos(pitch / 2);

	const Float yaw = radians.GetZ();
	const Float yawSin = Float::Sin(yaw / 2);
	const Float yawCos = Float::Cos(yaw / 2);

	m_w = (rollCos * pitchCos * yawCos) + (rollSin * pitchSin * yawSin);
	m_i = (rollSin * pitchCos * yawCos) - (rollCos * pitchSin * yawSin);
	m_j = (rollCos * pitchSin * yawCos) + (rollSin * pitchCos * yawSin);
	m_k = (rollCos * pitchCos * yawSin) - (rollSin * pitchSin * yawCos);

#endif // XYZ_OLD
	Normalize();
}

void Quaternion::ZYXRotation(const Vector3D& radians) {
	const Float roll = radians.GetX();
	const Float rollSin = Float::Sin(roll / 2);
	const Float rollCos = Float::Cos(roll / 2);

	const Float pitch = radians.GetY();
	const Float pitchSin = Float::Sin(pitch / 2);
	const Float pitchCos = Float::Cos(pitch / 2);

	const Float yaw = radians.GetZ();
	const Float yawSin = Float::Sin(yaw / 2);
	const Float yawCos = Float::Cos(yaw / 2);

	m_w = rollCos * pitchCos * yawCos + rollSin * pitchSin * yawSin;
	m_i = rollSin * pitchCos * yawCos - rollCos * pitchSin * yawSin;
	m_j = rollCos * pitchSin * yawCos + rollSin * pitchCos * yawSin;
	m_k = rollCos * pitchCos * yawSin - rollSin * pitchSin * yawCos;

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