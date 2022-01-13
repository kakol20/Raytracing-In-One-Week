#include "Camera.h"
#include <cmath>

Camera::Camera() {
	m_origin = Vector3D(0.0f, 0.0f, 0.0f);
	m_horizontal = Vector3D(2.0f, 0.0f, 0.0f);
	m_vertical = Vector3D(0.0f, (16.0f / 9.0f) * 2.0f, 0.0f);
	m_lowerLeftCorner = m_origin - (m_horizontal / 2.0f) - (m_vertical / 2.0f) - Vector3D(0.0f, 0.0f, 16.0f / 9.0f);
}

Camera::Camera(const float aspectRatio, const float vfov, const Vector3D lookFrom, const Vector3D lookAt, Vector3D up) {
	// calculate from fov
	float theta = Degrees2Radians(vfov);
	float h = tanf(theta / 2.0f);
	float vHeight = 2.0f * h;
	float vWidth = aspectRatio * vHeight;
	//float focalLength = 1.0f;

	// calculate position
	Vector3D w = lookFrom - lookAt;
	w = w.UnitVector();
	Vector3D u = up.CrossProduct(w);
	u = u.UnitVector();
	Vector3D v = w.CrossProduct(u);

	m_origin = lookFrom;
	m_horizontal = u * vWidth;
	m_vertical = v * vHeight;
	m_lowerLeftCorner = m_origin - (m_horizontal / 2.0f) - (m_horizontal / 2.0f) - w;
}

Camera::~Camera() {
}

Ray Camera::GetRay(const float s, const float t) {
	return Ray(m_origin, m_lowerLeftCorner + (m_horizontal * s) + (m_vertical * t) - m_origin);
}

const float Camera::Degrees2Radians(const float deg) {
	float PI = 3.14159265f;
	return deg * PI / 180.0f;
}
