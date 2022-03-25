//#include "ScaledObject.h"
//#include "RotatedObj.h"
//#include "TranslatedObj.h"

#include "TransformedObject.h"

TransformedObject::TransformedObject(const Vector3D scale, const Vector3D rotation, const Vector3D translation, Object* object) {
	m_object = object;
	m_scale = scale;
	m_rotation = rotation * Vector3D(3.14159265f / 180.f);
	m_rotation *= Vector3D(1.f, 1.f, -1.f);
	m_translation = translation;
	m_flipNormals = false;

	//m_scaledObject = new ScaledObject(scale, nullptr);
	//m_rotatedObject = new RotatedObj(rotation, m_object);
	//m_translatedObject = new TranslatedObj(translation, m_rotatedObject);
	//m_rotatedObject = new RotatedObj(rotation, m_object);
	
	//m_translatedObject = new TranslatedObj(translation, m_rotatedObject);

	m_xAxisRotation = Quaternion::AxisToRotation(Vector3D(1.f, 0.f, 0.f), m_rotation.GetX());
	m_xAxisRotation.Normalize();
	m_yAxisRotation = Quaternion::AxisToRotation(Vector3D(0.f, 1.f, 0.f), m_rotation.GetY());
	m_yAxisRotation.Normalize();
	m_zAxisRotation = Quaternion::AxisToRotation(Vector3D(0.f, 0.f, 1.f), m_rotation.GetZ());
	m_zAxisRotation.Normalize();
}

TransformedObject::TransformedObject(const bool flipNormals, const Vector3D rotation, const Vector3D translation, Object* object) {
	m_object = object;
	m_scale = Vector3D(1.f);
	m_rotation = rotation * Vector3D(3.14159265f / 180.f);
	m_rotation *= Vector3D(1.f, 1.f, -1.f);
	m_translation = translation;
	m_flipNormals = flipNormals;

	m_xAxisRotation = Quaternion::AxisToRotation(Vector3D(1.f, 0.f, 0.f), m_rotation.GetX());
	m_xAxisRotation.Normalize();
	m_yAxisRotation = Quaternion::AxisToRotation(Vector3D(0.f, 1.f, 0.f), m_rotation.GetY());
	m_yAxisRotation.Normalize();
	m_zAxisRotation = Quaternion::AxisToRotation(Vector3D(0.f, 0.f, 1.f), m_rotation.GetZ());
	m_zAxisRotation.Normalize();
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

bool TransformedObject::ScaleHit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	bool enableScaling = false;
	if (enableScaling) {
		Vector3D rayOrig = ray.GetOrig() / m_scale;
		Vector3D rayDir = ray.GetDir() / m_scale;
		rayDir.Normalize();

		Ray localRay(rayOrig, rayDir);
		HitRec temprec;
		if (!m_object->Hit(localRay, t_min, t_max * 2.f, temprec)) return false;

		Vector3D normal = temprec.GetNormal() / m_scale;
		normal.Normalize();
		Vector3D tangent = temprec.GetTangent() / m_scale;
		tangent.Normalize();

		Vector3D point = temprec.GetPoint() * m_scale;
		Vector3D diff = ray.GetOrig() - point;
		float magnitude = diff.Magnitude();

		if (t_min > magnitude || magnitude > t_max) return false;

		temprec.SetT(magnitude);
		temprec.SetPoint(point);
		temprec.SetNormal(normal);
		//temprec.SetTangents(tangent);

		rec = temprec;

		return true;
	}
	else {
		return m_object->Hit(ray, t_min, t_max, rec);
	}
	
}

bool TransformedObject::RotationHit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	if (m_object == nullptr) return false;

	Quaternion rayOrig = Quaternion::VectorToPure(ray.GetOrig());
	Quaternion rayDir = Quaternion::VectorToPure(ray.GetDir());

	Quaternion zInv = -m_zAxisRotation;
	Quaternion xInv = -m_xAxisRotation;
	Quaternion yInv = -m_yAxisRotation;

	// Z Axis rotation
	rayOrig = Quaternion::RotationQuat(zInv, rayOrig);
	rayDir = Quaternion::RotationQuat(zInv, rayDir);

	// X Axis rotation
	rayOrig = Quaternion::RotationQuat(xInv, Quaternion::VectorToPure(rayOrig.GetIJK()));
	rayDir = Quaternion::RotationQuat(xInv, Quaternion::VectorToPure(rayDir.GetIJK()));

	// Y Axis rotation
	rayOrig = Quaternion::RotationQuat(yInv, Quaternion::VectorToPure(rayOrig.GetIJK()));
	rayDir = Quaternion::RotationQuat(yInv, Quaternion::VectorToPure(rayDir.GetIJK()));

	// Calculate hit
	Ray localRay = Ray(rayOrig.GetIJK(), rayDir.GetIJK());
	if (!ScaleHit(localRay, t_min, t_max, rec)) return false;

	Quaternion point = Quaternion::VectorToPure(rec.GetPoint());
	Quaternion normal = Quaternion::VectorToPure(rec.GetNormal());
	Quaternion tangent = Quaternion::VectorToPure(rec.GetTangent());
	Quaternion bitangent = Quaternion::VectorToPure(rec.GetBitangent());

	// Y Axis rotation
	point = Quaternion::RotationQuat(m_yAxisRotation, point);
	normal = Quaternion::RotationQuat(m_yAxisRotation, normal);
	tangent = Quaternion::RotationQuat(m_yAxisRotation, tangent);
	bitangent = Quaternion::RotationQuat(m_yAxisRotation, bitangent);

	// X Axis rotation
	point = Quaternion::RotationQuat(m_xAxisRotation, Quaternion::VectorToPure(point.GetIJK()));
	normal = Quaternion::RotationQuat(m_xAxisRotation, Quaternion::VectorToPure(normal.GetIJK()));
	tangent = Quaternion::RotationQuat(m_xAxisRotation, Quaternion::VectorToPure(tangent.GetIJK()));
	bitangent = Quaternion::RotationQuat(m_xAxisRotation, Quaternion::VectorToPure(bitangent.GetIJK()));

	// Z Axis rotation
	point = Quaternion::RotationQuat(m_zAxisRotation, Quaternion::VectorToPure(point.GetIJK()));
	normal = Quaternion::RotationQuat(m_zAxisRotation, Quaternion::VectorToPure(normal.GetIJK()));
	tangent = Quaternion::RotationQuat(m_zAxisRotation, Quaternion::VectorToPure(tangent.GetIJK()));
	bitangent = Quaternion::RotationQuat(m_zAxisRotation, Quaternion::VectorToPure(bitangent.GetIJK()));

	rec.SetPoint(point.GetIJK());
	rec.SetNormal(normal.GetIJK());
	rec.SetTangent(tangent.GetIJK());
	rec.SetBitangent(bitangent.GetIJK());
	
	return true;
}

bool TransformedObject::TranslationHit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	if (m_object == nullptr) return false;

	Ray localRay(ray.GetOrig() - m_translation, ray.GetDir());

	if (!RotationHit(localRay, t_min, t_max, rec)) return false;

	rec.SetPoint(rec.GetPoint() + m_translation);

	if (m_flipNormals) {
		rec.SetFrontFace(!rec.GetFrontFace());
	}

	return true;
}

bool TransformedObject::SphereIntersectGround(const Vector3D pos, const float radius) {
	Vector3D newPos = pos;

	newPos -= m_translation;

	newPos = Quaternion::RotationVec(-m_zAxisRotation, newPos);
	newPos = Quaternion::RotationVec(-m_xAxisRotation, newPos);
	newPos = Quaternion::RotationVec(-m_yAxisRotation, newPos);

	return m_object->SphereIntersectGround(newPos, radius);
}

bool TransformedObject::SphereIntersectSphere(const Vector3D pos, const float radius) {
	Vector3D newPos = pos;

	newPos -= m_translation;
	return m_object->SphereIntersectSphere(newPos, radius);
}
