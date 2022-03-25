#include "TransformedSphere.h"

TransformedSphere::TransformedSphere(const float& radius, Material* mat, const Vector3D& rotation, const Vector3D& translation, bool flipNormals, const Vector3D uvScale) {
	m_radius = radius;
	m_mat = mat;
	m_flipNormals = flipNormals;
	m_pos = translation;

	Vector3D rotationRadians = (rotation * Vector3D::PI) / 180.f;

#ifdef MULTI_ROTATION
	m_rotation.reserve(3);
	m_rotation.push_back(Quaternion::AxisToRotation(Vector3D::XDir, rotationRadians.GetX()));
	m_rotation.push_back(Quaternion::AxisToRotation(Vector3D::YDir, rotationRadians.GetY()));
	m_rotation.push_back(Quaternion::AxisToRotation(Vector3D::ZDir, rotationRadians.GetZ()));

	for (auto it = m_rotation.begin(); it != m_rotation.end(); it++) {
		(*it).Normalize();
	}
#else
	std::vector<Quaternion> rotationVec;
	rotationVec.reserve(3);
	rotationVec.push_back(Quaternion::AxisToRotation(Vector3D::XDir, rotationRadians.GetX()));
	rotationVec.push_back(Quaternion::AxisToRotation(Vector3D::YDir, rotationRadians.GetY()));
	rotationVec.push_back(Quaternion::AxisToRotation(Vector3D::ZDir, rotationRadians.GetZ()));

	Vector3D v1(1.f);
	v1.Normalize();

	Vector3D v2 = v1;
	for (auto it = rotationVec.begin(); it != rotationVec.end(); it++) {
		v2 = Quaternion::RotationVec((*it), v2);
	}
	v2.Normalize();

	m_rotation = Quaternion::FromTwoPoints(v1, v2);
	m_rotationInv = Quaternion::FromTwoPoints(v2, v1);
#endif
}

TransformedSphere::~TransformedSphere() {
	m_mat = nullptr;
}

bool TransformedSphere::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	return TranslationHit(ray, t_min, t_max, rec);
}

bool TransformedSphere::SphereIntersectSphere(const Vector3D pos, const float radius) {
	Vector3D newPos = pos - m_pos;

#ifdef MULTI_ROTATION
	for (auto it = m_rotation.rbegin(); it != m_rotation.rend(); it++) {
		newPos = Quaternion::RotationVec(-(*it), newPos);
	}
#else
	newPos = Quaternion::RotationVec(m_rotationInv, newPos);
#endif	

	Vector3D sphereEdge = Vector3D(0.f) - newPos;
	sphereEdge.Normalize();
	sphereEdge *= radius;
	sphereEdge = newPos + sphereEdge;

	Vector3D overlap = sphereEdge;

	if (overlap.SqrMagnitude() < m_radius * m_radius) return true;

	return false;
}

bool TransformedSphere::TranslationHit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	Ray localRay(ray.GetOrig() - m_pos, ray.GetDir());
	if (!RotationHit(localRay, t_min, t_max, rec)) return false;

	rec.SetPoint(rec.GetPoint() + m_pos);

	if (m_flipNormals) {
		rec.SetFrontFace(!rec.GetFrontFace());
	}

	return true;
}

bool TransformedSphere::RotationHit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	Vector3D rayOrig = ray.GetOrig();
	Vector3D rayDir = ray.GetDir();

#ifdef MULTI_ROTATION
	for (auto it = m_rotation.rbegin(); it != m_rotation.rend(); it++) {
		rayOrig = Quaternion::RotationVec(-(*it), rayOrig);
		rayDir = Quaternion::RotationVec(-(*it), rayDir);
	}
#else
	rayOrig = Quaternion::RotationVec(m_rotationInv, rayOrig);
	rayDir = Quaternion::RotationVec(m_rotationInv, rayDir);
	rayDir.Normalize();
#endif

	Ray localRay = Ray(rayOrig, rayDir);
	if (!LocalHit(localRay, t_min, t_max, rec)) return false;

	Vector3D point = rec.GetPoint();
	Vector3D normal = rec.GetNormal();
	Vector3D tangent = rec.GetTangent();
	Vector3D bitangent = rec.GetBitangent();

#ifdef MULTI_ROTATION
	for (auto it = m_rotation.begin(); it != m_rotation.end(); it++) {
		point = Quaternion::RotationVec((*it), point);
		normal = Quaternion::RotationVec((*it), normal);
		tangent = Quaternion::RotationVec((*it), tangent);
		bitangent = Quaternion::RotationVec((*it), bitangent);
	}
#else
	point = Quaternion::RotationVec(m_rotation, point);
	normal = Quaternion::RotationVec(m_rotation, normal);
	tangent = Quaternion::RotationVec(m_rotation, tangent);
	bitangent = Quaternion::RotationVec(m_rotation, bitangent);

	normal.Normalize();
	tangent.Normalize();
	bitangent.Normalize();
#endif

	rec.SetPoint(point);
	rec.SetNormal(normal);
	rec.SetTangent(tangent);
	rec.SetBitangent(bitangent);

	return true;
}

bool TransformedSphere::LocalHit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	Vector3D oc = ray.GetOrig();
	float a = ray.GetDir().SqrMagnitude();
	float half_b = Vector3D::DotProduct(oc, ray.GetDir());
	float c = oc.SqrMagnitude() - m_radius * m_radius;

	float discriminant = half_b * half_b - a * c;

	if (discriminant < 0.f) return false;

	float sqrtd = sqrtf(discriminant);

	// Find the nearest root that lies in the acceptable range.
	float root = (-half_b - sqrtd) / a;

	Vector3D dist = ray.GetOrig() - ray.At(root);
	if (root < t_min || t_max < root/* || dist.Threshold(t_min)*/) {
		root = (-half_b + sqrtd) / a;

		dist = ray.GetOrig() - ray.At(root);
		if (root < t_min || t_max < root/* || dist.Threshold(t_min)*/) return false;
	}

	rec.SetT(root);
	rec.SetPoint(ray.At(root));
	rec.SetMat(m_mat);

	Vector3D outwardNormal = rec.GetPoint() / m_radius;
	rec.SetFaceNormal(ray, outwardNormal);

	rec.SetUV(CalculateUV(rec.GetPoint()) * m_uvScale);

	Vector3D tangent = CalculateTangent(rec);
	Vector3D bitangent = Vector3D::CrossProduct(rec.GetNormal(), tangent);

	rec.SetTangent(tangent);
	rec.SetBitangent(bitangent);
	return true;
}

Vector3D TransformedSphere::CalculateTangent(HitRec& rec) {
	Vector3D tangent = rec.GetNormal();
	tangent = Vector3D::CrossProduct(Vector3D::YDir, tangent);
	tangent.Normalize();
	return tangent;
}

Vector3D TransformedSphere::CalculateUV(const Vector3D point) {
	Vector3D dir = point / m_radius;

	float u, v;
	dir.UVSphere(u, v);

	//u = fmod(u - 0.25, 1.0);
	//v = 1.0 - fmod(v, 1.0);

	return Vector3D(u, v, 0.f);
}
