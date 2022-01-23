#include <math.h>

#include "Sphere.h"

Sphere::Sphere(const Vector3D center, const float radius, Material* mat) {
	m_pos = center;
	m_radius = radius;
	m_mat = mat;
}

bool Sphere::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	Vector3D oc = ray.GetOrigin() - m_pos;

	// https://raytracing.github.io/books/RayTracingInOneWeekend.html#addingasphere/creatingourfirstraytracedimage #5.2
	/*float a = ray.GetDirection().DotProduct(ray.GetDirection());
	float b = 2.0f * oc.DotProduct(ray.GetDirection());
	float c = oc.DotProduct(oc) - (m_radius * m_radius);
	float discriminant = b * b - 4 * a * c;*/

	// #6.2
	float a = ray.GetDirection().SqrMagnitude();
	float half_b = oc.DotProduct(ray.GetDirection());
	float c = oc.SqrMagnitude() - m_radius * m_radius;

	float discriminant = half_b * half_b - a * c;

	if (discriminant < 0.0f) return false;

	float sqrtd = sqrtf(discriminant);

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
	rec.SetPoint(ray.At(rec.GetT()));
	rec.SetMaterial(m_mat);

	Vector3D outwardNormal = (rec.GetPoint() - m_pos) / m_radius;
	rec.SetFaceNormal(ray, outwardNormal);

	return true;
}
