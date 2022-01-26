#include "Camera.h"
#include <cmath>

Camera::Camera() {
	m_origin = Vector3D(0.0f, 0.0f, 0.0f);
	m_horizontal = Vector3D(2.0f, 0.0f, 0.0f);
	m_vertical = Vector3D(0.0f, (16.0f / 9.0f) * 2.0f, 0.0f);
	m_lowerLeftCorner = m_origin - (m_horizontal / 2.0f) - (m_vertical / 2.0f) - Vector3D(0.0f, 0.0f, 16.0f / 9.0f);
}

Camera::Camera(const float aspectRatio, const float aperture, const float focusDist, const float vfov, Vector3D lookFrom, Vector3D lookAt, Vector3D vUp) {
	// calculate from fov
	const float theta = Degrees2Radians(vfov);
	const float h = tanf(theta / 2.0f);
	const float vHeight = 2.0f * h;
	const float vWidth = aspectRatio * vHeight;
	//float focalLength = 1.0f;

	// calculate position
	m_w = lookFrom - lookAt;
	m_w = m_w.UnitVector();

	m_u = vUp.CrossProduct(m_w);
	m_u = m_u.UnitVector();

	m_v = m_w.CrossProduct(m_u);
	//v = v.UnitVector();

	m_origin = lookFrom;

	m_horizontal = m_u * vWidth * focusDist;
	m_vertical = m_v * vHeight * focusDist;

	m_lowerLeftCorner = m_origin - (m_horizontal / 2.0f) - (m_vertical / 2.0f) - (m_w * focusDist);

	m_lensRadius = aperture / 2.0f;
}

Camera::~Camera() {
}

Ray Camera::GetRay(const float s, const float t) {
	//return Ray(m_origin, m_lowerLeftCorner + (m_horizontal * s) + (m_vertical * t) - m_origin);
	unsigned int bitCount = 32;
	Vector3D rd = Vector3D::RandomInUnitDisk(bitCount) * m_lensRadius;
	Vector3D offset = (m_u * rd.GetX()) + (m_v * rd.GetY());

	return Ray(m_origin + offset,
		m_lowerLeftCorner + (m_horizontal * s) + (m_vertical * t) - m_origin - offset);
}

const float Camera::Degrees2Radians(const float deg) {
	float PI = 3.14159265f;
	return deg * PI / 180.0f;
}
