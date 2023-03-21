#include "Ray.h"

Ray::Ray(const Vector3D& orig, const Vector3D& dir) {
	m_orig = orig;
	m_dir = dir;
}

Ray& Ray::operator=(const Ray& copyRay) {
	if (this == &copyRay) return *this;
	m_orig = copyRay.m_orig;
	m_dir = copyRay.m_dir;
	return *this;
}

Vector3D Ray::At(const Float t) const {
	return m_orig + (m_dir * t);
}