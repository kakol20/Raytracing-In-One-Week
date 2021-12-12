#include "Ray.h"

Ray::Ray() {
}

Ray::Ray(const Vector3D& orig, const Vector3D& dir) {
	m_orig = orig;
	m_dir = dir;
}

const Vector3D Ray::At(const float t) {
	return m_orig + (m_dir * t);
}

Ray::~Ray() {
}