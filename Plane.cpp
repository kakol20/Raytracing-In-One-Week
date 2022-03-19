#include <algorithm>
#include <math.h>
#include "Plane.h"

Plane::Plane(Plane::Type type, const Vector3D origin, const float width, const float height, Material* mat, const Vector3D uvScale) {
	m_height = height;
	m_mat = mat;
	m_pos = origin;
	m_type = type;
	m_uvScale = uvScale;
	m_width = width;

	const float PI = 3.14159265f;
	if (m_type == Plane::Type::XMinus) {
		m_rotation = Quaternion::AxisToRotation(Vector3D(0.f, 1.f, 0.f), -PI / 2.f);
	}
	else if (m_type == Plane::Type::XPlus) {
		m_rotation = Quaternion::AxisToRotation(Vector3D(0.f, 1.f, 0.f), PI / 2.f);
	}
	else if (m_type == Plane::Type::YMinus) {
		m_rotation = Quaternion::AxisToRotation(Vector3D(1.f, 0.f, 0.f), PI / 2.f);
	}
	else if (m_type == Plane::Type::YPlus) {
		m_rotation = Quaternion::AxisToRotation(Vector3D(1.f, 0.f, 0.f), -PI / 2.f);
	}
	else if (m_type == Plane::Type::ZMinus) {
		m_rotation = Quaternion::AxisToRotation(Vector3D(0.f, 1.f, 0.f), PI);
	}
	else {
		m_rotation = Quaternion::AxisToRotation(Vector3D(0.f, 1.f, 0.f), 0.f);
	}

	m_rotation.Normalize();
}

Plane::~Plane() {
	m_mat = nullptr;
}

bool Plane::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	Ray localRay = Ray(ray.GetOrig() - m_pos, ray.GetDir());

	Quaternion rotationInv = -m_rotation;

	Vector3D normal(0.f, 0.f, 1.f);
	normal = Quaternion::RotationVec(m_rotation, normal);

	float lDotN = Vector3D::DotProduct(localRay.GetDir(), normal);
	if (abs(lDotN) <= 1e-4f) return false; // account for floating point precision

	float d = Vector3D::DotProduct(Vector3D(0.f) - localRay.GetOrig(), normal) / lDotN;

	if (d <= t_min || d >= t_max) return false;

	Vector3D point = localRay.GetOrig() + (localRay.GetDir() * d);

	Vector3D maxPoint = Vector3D(m_width / 2, m_height / 2.f, 0.f);
	Vector3D minPoint = -maxPoint;

	Vector3D uvPoint = Quaternion::RotationVec(rotationInv, point);

	//if (!Vector3D::BoxBetween(uvPoint, minPoint, maxPoint, 1e-4f)) return false;
	if (!(abs(uvPoint.GetX()) <= maxPoint.GetX() && abs(uvPoint.GetY()) <= maxPoint.GetY())) return false;
	if (abs(uvPoint.GetZ()) > 1e-4f) return false;

	uvPoint *= (Vector3D(1.f, 1.f, 0.f));
	point += m_pos;

	rec.SetT(d);
	rec.SetPoint(point);
	rec.SetMat(m_mat);
	rec.SetFaceNormal(localRay, normal);

	// Normal Mapping
	Vector3D tangent = Vector3D(1.f, 0.f, 0.f);
	Vector3D bitangent = Vector3D(0.f, 1.f, 0.f);

	//if (m_type == Plane::Type::ZPlus || m_type == Plane::Type::ZMinus || m_type == Plane::Type::XMinus || m_type == Plane::Type::) bitangent *= -1.f;

	tangent = Quaternion::RotationVec(m_rotation, tangent);
	bitangent = Quaternion::RotationVec(m_rotation, bitangent);

	rec.SetTangent(tangent);
	rec.SetBitangent(bitangent);

	// UV Calculation
	Vector3D divide(fminf(m_width, m_height), fminf(m_width, m_height), 1.f);
	uvPoint -= minPoint;
	uvPoint /= divide;
	uvPoint = Vector3D(uvPoint.GetX(), -uvPoint.GetY());
	uvPoint *= m_uvScale;

	rec.SetUV(uvPoint);

	return true;
}