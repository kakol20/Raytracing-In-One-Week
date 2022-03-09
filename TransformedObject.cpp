//#include "ScaledObject.h"
//#include "RotatedObj.h"
//#include "TranslatedObj.h"

#include "TransformedObject.h"

TransformedObject::TransformedObject(const Vector3D rotation, const Vector3D translation, Object* object) {
	m_object = object;
	//m_scale = scale;
	m_rotation = rotation * Vector3D(3.14159265f / 180.f);
	m_translation = translation;

	//m_scaledObject = new ScaledObject(scale, nullptr);
	//m_rotatedObject = new RotatedObj(rotation, m_object);
	//m_translatedObject = new TranslatedObj(translation, m_rotatedObject);
	//m_rotatedObject = new RotatedObj(rotation, m_object);
	
	//m_translatedObject = new TranslatedObj(translation, m_rotatedObject);
}

TransformedObject::~TransformedObject() {
	//delete m_translatedObject;
	//m_translatedObject = nullptr;

	//delete m_rotatedObject;
	//m_rotatedObject = nullptr;
	
	//delete m_scaledObject;
	//m_scaledObject = nullptr;

	m_object = nullptr;
}

bool TransformedObject::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	if (m_object == nullptr) return false;

	//return m_translatedObject->Hit(ray, t_min, t_max, rec);

	return TranslationHit(ray, t_min, t_max, rec);
}

bool TransformedObject::RotationHit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	if (m_object == nullptr) return false;

	Vector3D xDir = Vector3D(1.f, 0.f, 0.f);
	Vector3D yDir = Vector3D(0.f, 1.f, 0.f);
	Vector3D zDir = Vector3D(0.f, 0.f, 1.f);

	// forward rotation
	Vector3D rayOrig = Vector3D::RotateAxis(ray.GetOrig(), zDir, -m_rotation.GetZ());
	Vector3D rayDir = Vector3D::RotateAxis(ray.GetDir(), zDir, -m_rotation.GetZ());

	// side rotation
	rayOrig = Vector3D::RotateAxis(rayOrig, xDir, -m_rotation.GetX());
	rayDir = Vector3D::RotateAxis(rayDir, xDir, -m_rotation.GetX());

	// up rotation
	rayOrig = Vector3D::RotateAxis(rayOrig, yDir, -m_rotation.GetY());
	rayDir = Vector3D::RotateAxis(rayDir, yDir, -m_rotation.GetY());

	Ray localRay(rayOrig, rayDir);
	if (!m_object->Hit(localRay, t_min, t_max, rec)) return false;

	Vector3D point = Vector3D::RotateAxis(rec.GetPoint(), yDir, m_rotation.GetY());
	Vector3D normal = Vector3D::RotateAxis(rec.GetNormal(), yDir, m_rotation.GetY());

	point = Vector3D::RotateAxis(point, xDir, m_rotation.GetX());
	normal = Vector3D::RotateAxis(normal, xDir, m_rotation.GetX());

	point = Vector3D::RotateAxis(point, zDir, m_rotation.GetZ());
	normal = Vector3D::RotateAxis(normal, zDir, m_rotation.GetZ());

	rec.SetPoint(point);
	rec.SetNormal(normal);

	return true;
}

bool TransformedObject::TranslationHit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	if (m_object == nullptr) return false;

	Ray localRay(ray.GetOrig() - m_translation, ray.GetDir());

	if (!RotationHit(localRay, t_min, t_max, rec)) return false;

	rec.SetPoint(rec.GetPoint() + m_translation);

	return true;
}

bool TransformedObject::SphereIntersectGround(const Vector3D pos, const float radius) {
	Vector3D xDir = Vector3D(1.f, 0.f, 0.f);
	Vector3D yDir = Vector3D(0.f, 1.f, 0.f);
	Vector3D zDir = Vector3D(0.f, 0.f, 1.f);

	Vector3D newPos = Vector3D::RotateAxis(pos, yDir, m_rotation.GetY());
	newPos = Vector3D::RotateAxis(pos, xDir, m_rotation.GetX());
	newPos = Vector3D::RotateAxis(pos, zDir, m_rotation.GetZ());

	newPos += m_translation;
	return m_object->SphereIntersectGround(newPos, radius);
}

bool TransformedObject::SphereIntersectSphere(const Vector3D pos, const float radius) {
	return m_object->SphereIntersectSphere(pos, radius);
}
