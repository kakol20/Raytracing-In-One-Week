#include "RotatedObj.h"

RotatedObj::RotatedObj(const Vector3D degrees, Object* object) {
	m_radians = degrees * Vector3D(3.14159265f / 180.f);
	m_radians *= Vector3D(1.f, 1.f, 1.f);
	m_object = object;
}

RotatedObj::~RotatedObj() {
	m_object = nullptr;
}

bool RotatedObj::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	if (m_object == nullptr) return false;
	Vector3D negativeRadians = -m_radians;

	Vector3D rayOrig = Vector3D::Rotate(ray.GetOrig(), negativeRadians);
	Vector3D rayDir = Vector3D::Rotate(ray.GetDir(), negativeRadians);

	Ray localRay(rayOrig, rayDir);

	if (!m_object->Hit(localRay, t_min, t_max, rec)) return false;

	Vector3D localPoint = Vector3D::Rotate(rec.GetPoint(), m_radians);
	Vector3D localNormal = rec.GetNormal();
	localNormal = Vector3D::Rotate(localNormal, m_radians);

	rec.SetPoint(localPoint);
	rec.SetNormal(localNormal);

	return true;
}

bool RotatedObj::SphereIntersectGround(const Vector3D pos, const float radius) {
	Vector3D l_pos = Vector3D::Rotate(pos, m_radians);
	return m_object->SphereIntersectGround(l_pos, radius);
}

bool RotatedObj::SphereIntersectSphere(const Vector3D pos, const float radius) {
	Vector3D l_pos = Vector3D::Rotate(pos, m_radians);
	return m_object->SphereIntersectSphere(l_pos, radius);
}