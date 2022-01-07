#include "Sphere.h"

Sphere::Sphere(const Vector3D center, const float radius) {
	m_pos = center;
	m_radius = radius;
}

bool Sphere::Hit(Ray& Hit) {
	Vector3D oc = Hit.GetOrigin() - m_pos;

	// https://raytracing.github.io/books/RayTracingInOneWeekend.html#addingasphere/creatingourfirstraytracedimage #5.2
	float a = Hit.GetDirection().DotProduct(Hit.GetDirection());
	float b = 2.0f * oc.DotProduct(Hit.GetDirection());
	float c = oc.DotProduct(oc) - (m_radius * m_radius);
	float discriminant = b * b - 4 * a * c;

	return discriminant > 0;
}
