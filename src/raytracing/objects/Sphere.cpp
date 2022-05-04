#include "Sphere.h"

Sphere::~Sphere() {
	m_mat = nullptr;
}

bool Sphere::Hit(Ray& ray, const Float& t_min, const Float& t_max, HitRec& rec) {
	if (TranslationHit(ray, t_min, t_max, rec)) {
		Vector3D normal = rec.GetNormal();
		Vector3D tangent = rec.GetTangent();
		Vector3D bitangent = rec.GetBitangent();

		normal.Normalize();
		tangent.Normalize();
		bitangent.Normalize();

		rec.SetNormal(normal);
		rec.SetTangent(tangent);
		rec.SetBitangent(bitangent);

		return true;
	}
	return false;
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
	Vector3D newPos = pos;
	newPos = m_rotationInv.RotateVector(newPos);

	Vector3D sphereEdge = Vector3D::Zero - newPos;
	sphereEdge.Normalize();
	sphereEdge *= radius;
	sphereEdge = newPos + sphereEdge;

	if (sphereEdge.SqrMagnitude() < radius * radius) return true;
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

	rec.SetPoint(m_rotation.RotateVector(rec.GetPoint(), true));
	rec.SetNormal(m_rotation.RotateVector(rec.GetNormal(), true));
	rec.SetTangent(m_rotation.RotateVector(rec.GetTangent(), true));
	rec.SetBitangent(m_rotation.RotateVector(rec.GetBitangent(), true));
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

	Float root = 0;

#define SPHERE_H_ROOT1

#ifdef SPHERE_H_ROOT1
	if (root < t_min || t_max < root) {
		root = (-half_b + sqrtd) / a;

		if (root < t_min || t_max < root) return false;
	}
#else
	Float root1 = (-half_b - sqrtd) / a;
	Float root2 = (-half_b + sqrtd) / a;

	if (root1 < 0) {
		root = root2;
	}
	else if (root1 < 0) {
		root = root1;
	}
	else {
		root = Float::Min(root1, root2);
	}

	if (root < t_min || t_max < root) return false;
#endif // SPHERE_H_ROOT1

	rec.SetT(root);
	rec.SetPoint(ray.At(root));
	rec.SetMat(m_mat);

	Vector3D outwardNormal = rec.GetPoint() / m_radius;
	outwardNormal.Normalize();
	rec.SetFaceNormal(ray, outwardNormal);

	rec.SetUV(CalculateUV(rec.GetPoint()) * m_uvScale);

	Vector3D tangent = CalculateTangent(rec);
	Vector3D bitangent = Vector3D::CrossProduct(rec.GetNormal(), tangent);

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