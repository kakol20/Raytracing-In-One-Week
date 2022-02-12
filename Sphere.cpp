#include <cmath>

#include "Sphere.h"

Sphere::Sphere() : Sphere(Vector3D(), 1.f, nullptr) {
}

Sphere::Sphere(const Vector3D center, const float radius, Material* mat, const float uvScale) {
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
	if (root < t_min || t_max < root) {
		root = (-half_b + sqrtd) / a;

		if (root < t_min || t_max < root) return false;
	}

	/*rec.t = root;
	rec.point = ray.At(rec.t);
	Vector3D outwardNormal = (rec.point - m_pos) / m_radius;*/
	rec.SetT(root);
	rec.SetPoint(ray.At(root));
	rec.SetMat(m_mat);

	Vector3D outwardNormal = (rec.GetPoint() - m_pos) / m_radius;
	rec.SetFaceNormal(ray, outwardNormal);

	rec.SetUV(CalculateUV(rec.GetPoint()) * m_uvScale);
	
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

Vector3D Sphere::CalculateUV(const Vector3D point) {
	Vector3D dir = point - m_pos;
	dir.Normalize();

	float u, v;
	dir.UVSphere(u, v);

	//u = fmod(u - 0.25, 1.0);
	//v = 1.0 - fmod(v, 1.0);

	return Vector3D(u, v, 0.f);
}