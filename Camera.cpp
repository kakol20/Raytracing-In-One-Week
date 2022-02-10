#include <math.h>

#include "Camera.h"

Camera::Camera() : Camera(1.f, 0.f, 10.f, 90.f, Vector3D(0.f, 0.f, 0.f), Vector3D(0.f, 0.f, 1.f), Vector3D(0.f, 1.f, 0.f)) {
}

Camera::Camera(const float aspectRatio, const float aperture, const float focusDist, const float vFOV, const Vector3D lookFrom, const Vector3D lookAt, const Vector3D up) {
	float PI = 3.141592653f;

	// calculate from FOV
	const float theta = vFOV * (PI / 180.f);
	const float h = tanf(theta / 2.f);
	const float vHeight = 2.f * h;
	const float vWidth = aspectRatio * vHeight;

	// calculate position
	m_w = lookFrom - lookAt;
	m_w.Normalize();

	m_u = Vector3D::CrossProduct(up, m_w);
	m_u.Normalize();

	m_v = Vector3D::CrossProduct(m_w, m_u);

	m_origin = lookFrom;

	m_horizontal = m_u * vWidth * focusDist;
	m_vertical = m_v * vHeight * focusDist;

	m_lowerLeftCorner = m_origin - (m_horizontal / 2.f) - (m_vertical / 2.f) - (m_w * focusDist);

	m_lensRadius = aperture / 2.f;
}

Camera::~Camera() {
}

Camera& Camera::operator=(const Camera& copyCamera) {
	if (this == &copyCamera) return *this;

	m_horizontal = copyCamera.m_horizontal;
	m_lensRadius = copyCamera.m_lensRadius;
	m_lowerLeftCorner = copyCamera.m_lowerLeftCorner;
	m_origin = copyCamera.m_origin;
	m_u = copyCamera.m_u;
	m_v = copyCamera.m_v;
	m_vertical = copyCamera.m_vertical;
	m_w = copyCamera.m_w;

	return *this;
}

Ray Camera::GetRay(const float s, const float t) const {
	Vector3D rd = Vector3D::RandomInUnitDisk() * m_lensRadius;
	Vector3D offset = (m_u * rd.GetX()) + (m_v * rd.GetY());

	Vector3D dir = m_lowerLeftCorner + (m_horizontal * s) + (m_vertical * t) - m_origin - offset;
	dir.Normalize();

	return Ray(m_origin + offset, dir);
}
