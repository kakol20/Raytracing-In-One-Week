#include "Ray.h"

Ray::Ray() {
	m_orig = Vector3D(0.0f, 0.0f, 0.0f);
	m_dir = Vector3D(0.0f, 0.0f, 0.0f);
}

Ray::Ray(const Vector3D orig, const Vector3D dir) {
	m_orig = orig;
	m_dir = dir;
}

Ray& Ray::operator=(const Ray& copyRay) {
	// TODO: insert return statement here
	if (&copyRay == this) return *this;

	m_orig = copyRay.m_orig;
	m_dir = copyRay.m_dir;

	return *this;
}

const Vector3D Ray::At(const float t) {
	return m_orig + (m_dir * t);
}

Ray::~Ray() {
}