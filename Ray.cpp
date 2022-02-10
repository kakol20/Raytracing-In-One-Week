#include "Ray.h"

Ray::Ray() : Ray(Vector3D(), Vector3D(1.f, 0.f, 0.f)) {
}

Ray::Ray(const Vector3D& orig, const Vector3D& dir) {
	m_orig = orig;
	m_dir = dir;
}

Ray& Ray::operator=(const Ray& copyRay) {
	if (&copyRay == this) return *this;

	m_dir = copyRay.m_dir;
	m_orig = copyRay.m_orig;

	return *this;
}

Vector3D Ray::At(const float t) {
	return m_orig + (m_dir * t);
}
