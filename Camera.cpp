#include "Camera.h"

Camera::Camera() {
	m_origin = Vector3D(0.0f, 0.0f, 0.0f);
	m_horizontal = Vector3D(2.0f, 0.0f, 0.0f);
	m_vertical = Vector3D(0.0f, (16.0f / 9.0f) * 2.0f, 0.0f);
	m_lowerLeftCorner = m_origin - (m_horizontal / 2.0f) - (m_vertical / 2.0f) - Vector3D(0.0f, 0.0f, 16.0f / 9.0f);
}

Camera::Camera(const float aspectRatio, const float vHeight, const float vWidth, const float focalLength) {
	m_origin = Vector3D(0.0f, 0.0f, 0.0f);
	m_horizontal = Vector3D(vWidth, 0.0f, 0.0f);
	m_vertical = Vector3D(0.0f, vHeight, 0.0f);
	m_lowerLeftCorner = m_origin - (m_horizontal / 2.0f) - (m_vertical / 2.0f) - Vector3D(0.0f, 0.0f, focalLength);
}

Camera::~Camera() {
}

Ray Camera::GetRay(const float u, const float v) {
	return Ray(m_origin, m_lowerLeftCorner + (m_horizontal * u) + (m_vertical * v) - m_origin);
}
