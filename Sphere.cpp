#include "Sphere.h"
#include <math.h>;

Sphere::Sphere(const Vector3D center, const float radius) {
	m_pos = center;
	m_radius = radius;
}

float Sphere::Hit(Ray & ray) {
	Vector3D oc = ray.GetOrigin() - m_pos;

	// https://raytracing.github.io/books/RayTracingInOneWeekend.html#addingasphere/creatingourfirstraytracedimage #5.2
	float a = ray.GetDirection().DotProduct(ray.GetDirection());
	float b = 2.0f * oc.DotProduct(ray.GetDirection());
	float c = oc.DotProduct(oc) - (m_radius * m_radius);
	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0) {
		return -1.0f;
	}
	else {
		return (-b - sqrtf(discriminant)) / (2.0f * a);
	}

	return 0.0f;
}

Vector3D Sphere::RayColor(Ray & ray) {
	float t = Hit(ray);

	// https://raytracing.github.io/books/RayTracingInOneWeekend.html#surfacenormalsandmultipleobjects
	if (t > 0.0) {
		Vector3D N = ray.At(t) - Vector3D(0, 0, -1);
		N.UnitVector();

		N = Vector3D(N.GetX() + 1, N.GetY() + 1, N.GetZ() + 1) * 0.5f;
		N *= 255.0f;

		return N;
	}
	return Vector3D();
}
