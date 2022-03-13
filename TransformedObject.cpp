//#include "ScaledObject.h"
//#include "RotatedObj.h"
//#include "TranslatedObj.h"
#include "Quaternion.h"

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
}

TransformedObject::TransformedObject(const bool flipNormals, const Vector3D rotation, const Vector3D translation, Object* object) {
	m_object = object;
	m_scale = Vector3D(1.f);
	m_rotation = rotation * Vector3D(3.14159265f / 180.f);
	m_rotation *= Vector3D(1.f, 1.f, -1.f);
	m_translation = translation;
	m_flipNormals = false;
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

bool TransformedObject::RotationHitV1(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
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
	if (!ScaleHit(localRay, t_min, t_max, rec)) return false;

	Vector3D point = Vector3D::RotateAxis(rec.GetPoint(), yDir, m_rotation.GetY());
	Vector3D normal = Vector3D::RotateAxis(rec.GetNormal(), yDir, m_rotation.GetY());
	Vector3D tangent = Vector3D::RotateAxis(rec.GetTangent(), yDir, m_rotation.GetY());
	Vector3D bitangent = Vector3D::RotateAxis(rec.GetBitangent(), yDir, m_rotation.GetY());

	point = Vector3D::RotateAxis(point, xDir, m_rotation.GetX());
	normal = Vector3D::RotateAxis(normal, xDir, m_rotation.GetX());
	tangent = Vector3D::RotateAxis(tangent, xDir, m_rotation.GetX());
	bitangent = Vector3D::RotateAxis(bitangent, xDir, m_rotation.GetX());

	point = Vector3D::RotateAxis(point, zDir, m_rotation.GetZ());
	normal = Vector3D::RotateAxis(normal, zDir, m_rotation.GetZ());
	tangent = Vector3D::RotateAxis(tangent, zDir, m_rotation.GetZ());
	bitangent = Vector3D::RotateAxis(bitangent, zDir, m_rotation.GetZ());

	rec.SetPoint(point);
	rec.SetNormal(normal);
	rec.SetTangent(tangent);
	rec.SetBitangent(bitangent);

	return true;
}

bool TransformedObject::RotationHitV2(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	if (m_object == nullptr) return false;

	Quaternion rotation = Quaternion::ToQuaternionYXZ(m_rotation.GetY(), m_rotation.GetX(), m_rotation.GetZ());
	rotation.Normalize();
	rotation.Conjugate();

	// Rotate ray
	Quaternion rayOrig = Quaternion::RotationQuat(rotation, Quaternion::VectorToPure(ray.GetOrig()));
	Quaternion rayDir = Quaternion::RotationQuat(rotation, Quaternion::VectorToPure(ray.GetDir()));

	// Calculate hit
	Ray localRay(rayOrig.GetIJK(), rayDir.GetIJK());
	if (!ScaleHit(localRay, t_min, t_max, rec)) return false;

	// Rotate
	rotation.Conjugate();
	Quaternion point = Quaternion::RotationQuat(rotation, Quaternion::VectorToPure(rec.GetPoint()));
	Quaternion normal = Quaternion::RotationQuat(rotation, Quaternion::VectorToPure(rec.GetNormal()));
	Quaternion tangent = Quaternion::RotationQuat(rotation, Quaternion::VectorToPure(rec.GetTangent()));
	Quaternion bitangent = Quaternion::RotationQuat(rotation, Quaternion::VectorToPure(rec.GetBitangent()));

	Vector3D normalVec = normal.GetIJK();
	normal.Normalize();
	Vector3D tangentVec = tangent.GetIJK();
	tangentVec.Normalize();
	Vector3D bitangentVec = bitangent.GetIJK();
	bitangentVec.Normalize();

	rec.SetPoint(point.GetIJK());
	rec.SetNormal(normalVec);
	rec.SetTangent(tangentVec);
	rec.SetBitangent(bitangentVec);

	return true;
}

bool TransformedObject::RotationHitV3(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	if (m_object == nullptr) return false;

	Quaternion rayOrig = Quaternion::VectorToPure(ray.GetOrig());
	Quaternion rayDir = Quaternion::VectorToPure(ray.GetDir());

	// Z Axis rotation
	Quaternion zAxisRot = Quaternion::AxisToRotation(Vector3D(0.f, 0.f, 1.f), -m_rotation.GetZ());
	rayOrig = Quaternion::RotationQuat(zAxisRot, rayOrig);
	rayDir = Quaternion::RotationQuat(zAxisRot, rayDir);

	// X Axis rotation
	Quaternion xAxisRot = Quaternion::AxisToRotation(Vector3D(1.f, 0.f, 0.f), -m_rotation.GetX());
	rayOrig = Quaternion::RotationQuat(xAxisRot, Quaternion::VectorToPure(rayOrig.GetIJK()));
	rayDir = Quaternion::RotationQuat(xAxisRot, Quaternion::VectorToPure(rayDir.GetIJK()));

	// Y Axis rotation
	Quaternion yAxisRot = Quaternion::AxisToRotation(Vector3D(0.f, 1.f, 0.f), -m_rotation.GetY());
	rayOrig = Quaternion::RotationQuat(yAxisRot, Quaternion::VectorToPure(rayOrig.GetIJK()));
	rayDir = Quaternion::RotationQuat(yAxisRot, Quaternion::VectorToPure(rayDir.GetIJK()));

	// Calculate hit
	Ray localRay = Ray(rayOrig.GetIJK(), rayDir.GetIJK());
	if (!ScaleHit(localRay, t_min, t_max, rec)) return false;

	Quaternion point = Quaternion::VectorToPure(rec.GetPoint());
	Quaternion normal = Quaternion::VectorToPure(rec.GetNormal());
	Quaternion tangent = Quaternion::VectorToPure(rec.GetTangent());
	Quaternion bitangent = Quaternion::VectorToPure(rec.GetBitangent());

	// Y Axis rotation
	yAxisRot.Conjugate();
	point = Quaternion::RotationQuat(yAxisRot, point);
	normal = Quaternion::RotationQuat(yAxisRot, normal);
	tangent = Quaternion::RotationQuat(yAxisRot, tangent);
	bitangent = Quaternion::RotationQuat(yAxisRot, bitangent);

	// X Axis rotation
	xAxisRot.Conjugate();
	point = Quaternion::RotationQuat(xAxisRot, Quaternion::VectorToPure(point.GetIJK()));
	normal = Quaternion::RotationQuat(xAxisRot, Quaternion::VectorToPure(normal.GetIJK()));
	tangent = Quaternion::RotationQuat(xAxisRot, Quaternion::VectorToPure(tangent.GetIJK()));
	bitangent = Quaternion::RotationQuat(xAxisRot, Quaternion::VectorToPure(bitangent.GetIJK()));

	// Z Axis rotation
	zAxisRot.Conjugate();
	point = Quaternion::RotationQuat(zAxisRot, Quaternion::VectorToPure(point.GetIJK()));
	normal = Quaternion::RotationQuat(zAxisRot, Quaternion::VectorToPure(normal.GetIJK()));
	tangent = Quaternion::RotationQuat(zAxisRot, Quaternion::VectorToPure(tangent.GetIJK()));
	bitangent = Quaternion::RotationQuat(zAxisRot, Quaternion::VectorToPure(bitangent.GetIJK()));

	rec.SetPoint(point.GetIJK());
	rec.SetNormal(normal.GetIJK());
	rec.SetTangent(tangent.GetIJK());
	rec.SetBitangent(bitangent.GetIJK());
	
	return true;
}

bool TransformedObject::TranslationHit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	if (m_object == nullptr) return false;

	Ray localRay(ray.GetOrig() - m_translation, ray.GetDir());

	if (!RotationHitV1(localRay, t_min, t_max, rec)) return false;
	//if (!RotationHitV2(localRay, t_min, t_max, rec)) return false;
	//if (!RotationHitV3(localRay, t_min, t_max, rec)) return false;

	rec.SetPoint(rec.GetPoint() + m_translation);

	if (m_flipNormals) {
		rec.SetFrontFace(!rec.GetFrontFace());
	}

	return true;
}

bool TransformedObject::SphereIntersectGround(const Vector3D pos, const float radius) {
	Vector3D xDir = Vector3D(1.f, 0.f, 0.f);
	Vector3D yDir = Vector3D(0.f, 1.f, 0.f);
	Vector3D zDir = Vector3D(0.f, 0.f, -1.f);

	Vector3D newPos = pos;

	newPos -= m_translation;

	newPos = Vector3D::RotateAxis(pos, zDir, m_rotation.GetZ());
	newPos = Vector3D::RotateAxis(pos, xDir, m_rotation.GetX());
	newPos = Vector3D::RotateAxis(pos, yDir, m_rotation.GetY());

	return m_object->SphereIntersectGround(newPos, radius);
}

bool TransformedObject::SphereIntersectSphere(const Vector3D pos, const float radius) {
	Vector3D newPos = pos;

	newPos -= m_translation;
	return m_object->SphereIntersectSphere(newPos, radius);
}
