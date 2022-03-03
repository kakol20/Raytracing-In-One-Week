#include "TranslatedObj.h"

TranslatedObj::TranslatedObj(const Vector3D translation, Object* object) {
	m_pos = translation;
	m_object = object;
}

TranslatedObj::~TranslatedObj() {
	m_object = nullptr;
}

bool TranslatedObj::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	Ray localRay = Ray(ray.GetOrig() - m_pos, ray.GetDir());

	if (!m_object->Hit(localRay, t_min, t_max, rec)) return false;

	rec.SetPoint(rec.GetPoint() + m_pos);

	Vector3D normal = rec.GetNormal();
	rec.SetFaceNormal(localRay, normal);

	return true;
}

bool TranslatedObj::SphereIntersectGround(const Vector3D pos, const float radius) {
	return m_object->SphereIntersectGround(pos, radius);
}

bool TranslatedObj::SphereIntersectSphere(const Vector3D pos, const float radius) {
	return m_object->SphereIntersectSphere(pos, radius);
}
