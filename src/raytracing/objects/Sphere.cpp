#include "Sphere.h"

Sphere::~Sphere() {
	m_mat = nullptr;
}

bool Sphere::Hit(Ray& ray, const Float& t_min, const Float& t_max, HitRec& rec) {
	return TranslationHit(ray, t_min, t_max, rec);
}

Sphere::Sphere(const Float& radius, Material* mat, const Vector3D& rotation, const Vector3D& translation, bool flipNormals, const Vector3D uvScale) {
	m_rotation.XYZRotation(rotation * Float::ToRadians);
	m_rotationInv = m_rotation;
	m_rotationInv.Conjugate();

	m_radius = radius;
	m_mat = mat;
	m_flipNormals = flipNormals;
	m_pos = translation;
}

bool Sphere::SphereIntersectSphere(const Vector3D& pos, const Float& radius) {
	Vector3D newPos = pos - m_pos;
	//newPos = m_rotationInv.RotateVector(newPos);

	Vector3D sphereEdge = Vector3D::Zero - newPos;
	sphereEdge.Normalize();
	sphereEdge *= radius;
	sphereEdge = newPos + sphereEdge;

	if (sphereEdge.SqrMagnitude() < (m_radius * m_radius)) return true;
	return false;
}

bool Sphere::TranslationHit(Ray& ray, const Float& t_min, const Float& t_max, HitRec& rec) {
	Ray localRay(ray.GetOrig() - m_pos, ray.GetDir());
	if (!RotationHit(localRay, t_min, t_max, rec)) return false;

	rec.SetPoint(rec.GetPoint() + m_pos);

	if (m_flipNormals) {
		rec.SetFrontFace(!rec.GetFrontFace());
	}

	return true;
}

bool Sphere::RotationHit(Ray& ray, const Float& t_min, const Float& t_max, HitRec& rec) {
	Vector3D rayOrig = m_rotationInv.RotateVector(ray.GetOrig());
	Vector3D rayDir = m_rotationInv.RotateVector(ray.GetDir());

	Ray localRay = Ray(rayOrig, rayDir);
	if (!LocalHit(localRay, t_min, t_max, rec)) return false;

	Vector3D point = m_rotation.RotateVector(rec.GetPoint());
	Vector3D normal = m_rotation.RotateVector(rec.GetNormal());
	Vector3D tangent = m_rotation.RotateVector(rec.GetTangent());
	Vector3D bitangent = m_rotation.RotateVector(rec.GetBitangent());

	/*normal.Normalize();
	tangent.Normalize();
	bitangent.Normalize();*/

	rec.SetPoint(point);
	rec.SetNormal(normal);
	rec.SetTangent(tangent);
	rec.SetBitangent(bitangent);
	return true;
}

bool Sphere::LocalHit(Ray& ray, const Float& t_min, const Float& t_max, HitRec& rec) {
	Vector3D oc = ray.GetOrig();
	Float a = ray.GetDir().SqrMagnitude();
	Float half_b = Vector3D::DotProduct(oc, ray.GetDir());
	Float c = oc.SqrMagnitude() - (m_radius * m_radius);

	Float discriminant = half_b * half_b - a * c;

	if (discriminant < 0) return false;

	Float sqrtd = Float::Sqrt(discriminant);

	// Find the nearest root that lies in the acceptable range.

	Float root = (-half_b - sqrtd) / a;

	if (root < t_min || t_max < root) {
		root = (-half_b + sqrtd) / a;

		if (root < t_min || t_max < root) return false;
	}

	rec.SetT(root);
	rec.SetPoint(ray.At(root));
	rec.SetMat(m_mat);

	Vector3D outwardNormal = rec.GetPoint() / m_radius;
	outwardNormal.Normalize();
	rec.SetFaceNormal(ray, outwardNormal);

	//rec.SetFrontFace(m_flipNormals ? !rec.GetFrontFace() : rec.GetFrontFace());

	rec.SetUV(CalculateUV(rec.GetPoint()) * m_uvScale);

	Vector3D tangent = CalculateTangent(rec);
	Vector3D bitangent = Vector3D::CrossProduct(outwardNormal, tangent);

	rec.SetTangent(tangent);
	rec.SetBitangent(bitangent);
	return true;
}

Vector3D Sphere::CalculateTangent(HitRec& rec) {
	Vector3D tangent = rec.GetNormal();
	tangent = Vector3D::CrossProduct(Vector3D::Up, tangent);
	tangent.Normalize();
	return tangent;
}

Vector3D Sphere::CalculateUV(const Vector3D& point) {
	Vector3D dir = point / m_radius;

	Vector3D uv = dir.UVSphere();

	return Vector3D(uv.GetX(), uv.GetY(), 0);
}