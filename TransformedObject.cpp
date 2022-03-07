//#include "ScaledObject.h"
#include "RotatedObj.h"
#include "TranslatedObj.h"

#include "TransformedObject.h"

TransformedObject::TransformedObject(const Vector3D rotation, const Vector3D translation, Object* object) {
	m_object = object;
	//m_scale = scale;
	//m_rotation = rotation * Vector3D(3.14159265f / 180.f);
	//m_translation = translation;

	//m_scaledObject = new ScaledObject(scale, nullptr);
	//m_rotatedObject = new RotatedObj(rotation, m_object);
	//m_translatedObject = new TranslatedObj(translation, m_rotatedObject);
	m_xRotatedObject = new RotatedObj(rotation * Vector3D(1.f, 0.f, 0.f), m_object);
	m_yRotatedObject = new RotatedObj(rotation * Vector3D(0.f, 1.f, 0.f), m_xRotatedObject);
	m_zRotatedObject = new RotatedObj(rotation * Vector3D(0.f, 0.f, 1.f), m_yRotatedObject);
	
	m_translatedObject = new TranslatedObj(translation, m_zRotatedObject);
}

TransformedObject::~TransformedObject() {
	delete m_translatedObject;
	m_translatedObject = nullptr;

	delete m_zRotatedObject;
	m_zRotatedObject = nullptr;

	delete m_yRotatedObject;
	m_yRotatedObject = nullptr;

	delete m_xRotatedObject;
	m_xRotatedObject = nullptr;
	
	//delete m_scaledObject;
	//m_scaledObject = nullptr;

	m_object = nullptr;
}

bool TransformedObject::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	if (m_object == nullptr) return false;

	return m_translatedObject->Hit(ray, t_min, t_max, rec);
}

bool TransformedObject::SphereIntersectGround(const Vector3D pos, const float radius) {
	return m_translatedObject->SphereIntersectGround(pos, radius);
}

bool TransformedObject::SphereIntersectSphere(const Vector3D pos, const float radius) {
	return m_translatedObject->SphereIntersectSphere(pos, radius);
}
