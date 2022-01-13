#include "Camera.h"
#include <cmath>

Camera::Camera() {
	m_origin = Vector3D(0.0f, 0.0f, 0.0f);
	m_horizontal = Vector3D(2.0f, 0.0f, 0.0f);
	m_vertical = Vector3D(0.0f, (16.0f / 9.0f) * 2.0f, 0.0f);
	m_lowerLeftCorner = m_origin - (m_horizontal / 2.0f) - (m_vertical / 2.0f) - Vector3D(0.0f, 0.0f, 16.0f / 9.0f);
}

Camera::Camera(const float aspectRatio, const float vfov, Vector3D lookFrom, Vector3D lookAt, Vector3D vUp) {
	// calculate from fov
	const float theta = Degrees2Radians(vfov);
	const float h = tanf(theta / 2.0f);
	const float vHeight = 2.0f * h;
	const float vWidth = aspectRatio * vHeight;
	//float focalLength = 1.0f;

	// calculate position
	Vector3D w = lookFrom;
	w -= lookAt;
	w = w.UnitVector();
	Vector3D u = vUp.CrossProduct(w);
	u = u.UnitVector();
	Vector3D v = w.CrossProduct(u);
	//v = v.UnitVector();

	m_origin = lookFrom;

	m_horizontal = u;
	m_horizontal *= vWidth;

	m_vertical = v;
	m_vertical *= vHeight;

	m_lowerLeftCorner = m_origin - (m_horizontal / 2.0f) - (m_vertical / 2.0f) - w;
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
