#include <cmath>

#include "Sphere.h"

Sphere::Sphere() : Sphere(Vector3D(), 1.f, nullptr) {
}

Sphere::Sphere(const Vector3D center, const float radius, Material* mat, const Vector3D uvScale) {
	m_pos = center;
	m_radius = radius;
	m_mat = mat;
	m_uvScale = uvScale;
}

Sphere::~Sphere() {
}

bool Sphere::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	Vector3D oc = ray.GetOrig() - m_pos;
	float a = ray.GetDir().SqrMagnitude();
	float half_b = Vector3D::DotProduct(oc, ray.GetDir());
	float c = oc.SqrMagnitude() - m_radius * m_radius;

	float discriminant = half_b * half_b - a * c;

	if (discriminant < 0.f) return false;

	float sqrtd = sqrt(discriminant);

	// Find the nearest root that lies in the acceptable range.
	float root = (-half_b - sqrtd) / a;
	/*if (root < t_min || t_max < root) {
		root = (-half_b + sqrtd) / a;

		if (root < t_min || t_max < root) return false;
	}*/

	Vector3D dist = ray.GetOrig() - ray.At(root);
	if (root < t_min || t_max < root || dist.Threshold(t_min)) {
		root = (-half_b + sqrtd) / a;

		dist = ray.GetOrig() - ray.At(root);
		if (root < t_min || t_max < root || dist.Threshold(t_min)) return false;
	}
	
	rec.SetT(root);
	rec.SetPoint(ray.At(root));
	rec.SetMat(m_mat);

	Vector3D outwardNormal = (rec.GetPoint() - m_pos) / m_radius;
	rec.SetFaceNormal(ray, outwardNormal);

	rec.SetUV(CalculateUV(rec.GetPoint()) * m_uvScale);
	rec.SetTangents(CalculateTangent(rec));
	return true;
}

bool Sphere::SphereIntersectGround(const Vector3D pos, const float radius) {
	return false;
}

bool Sphere::SphereIntersectSphere(const Vector3D pos, const float radius) {
	Vector3D sphereEdge = m_pos - pos;
	sphereEdge.Normalize();
	sphereEdge *= radius;
	sphereEdge = pos + sphereEdge;

	Vector3D overlap = sphereEdge - m_pos;

	if (overlap.SqrMagnitude() < m_radius * m_radius) return true;

	return false;
}

Vector3D Sphere::CalculateTangent(HitRec& rec) {
	//Vector3D P = rec.GetNormal();
	Vector3D tangent = rec.GetNormal();
	//Vector3D A(0.f, 0.f, 1.f);
	Vector3D A(0.f, 1.f, 0.f);
	tangent = Vector3D::CrossProduct(A, tangent);
	tangent.Normalize();
	return tangent * 1.f;
}

Vector3D Sphere::CalculateUV(const Vector3D point) {
	Vector3D dir = point - m_pos;
	dir.Normalize();

	float u, v;
	dir.UVSphere(u, v);

	//u = fmod(u - 0.25, 1.0);
	//v = 1.0 - fmod(v, 1.0);

	return Vector3D(u, v, 0.f);
}