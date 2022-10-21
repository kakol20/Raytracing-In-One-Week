#include <cmath>

#include "../../maths/Maths.h"

#include "Camera.h"

Camera::Camera(const Camera& copyCamera) {
	m_horizontal = copyCamera.m_horizontal;
	m_lensRadius = copyCamera.m_lensRadius;
	m_lowerLeftCorner = copyCamera.m_lowerLeftCorner;
	m_origin = copyCamera.m_origin;
	m_u = copyCamera.m_u;
	m_v = copyCamera.m_v;
	m_vertical = copyCamera.m_vertical;
	m_w = copyCamera.m_w;
}

Camera::Camera(const float& aspectRatio, const float& blurStrength, const float& focusDist, const float& vFOV, const sf::Vector3f& lookFrom, const sf::Vector3f& lookAt, const sf::Vector3f& up) {
	const float theta = vFOV * Maths::ToRadians;
	const float h = std::tanf(theta / 2);
	const float vHeight = 2 * h;
	const float vWidth = aspectRatio * vHeight;

	// calculate position
	m_w = lookFrom - lookAt;
	m_w = rt::Vector3::Normalize(m_w);

	m_u = rt::Vector3::CrossProduct(up, m_w);
	m_u = rt::Vector3::Normalize(m_u);

	m_v = rt::Vector3::CrossProduct(m_w, m_u);
	m_v = rt::Vector3::Normalize(m_v);

	m_origin = lookFrom;

	m_horizontal = m_u * vWidth * focusDist;
	m_vertical = m_v * vHeight * focusDist;

	m_lowerLeftCorner = m_origin - (m_horizontal / 2.f) - (m_vertical / 2.f) - (m_w * focusDist);

	m_lensRadius = blurStrength / 2;
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

Ray Camera::GetRay(const float& s, const float& t) const {
	sf::Vector3f rd = rt::Vector3::RandomInUnitDisk() * m_lensRadius;
	sf::Vector3f offset = (m_u * rd.x) + (m_v * rd.y);

	sf::Vector3f dir = m_lowerLeftCorner + (m_horizontal * s) + (m_vertical * t) - m_origin - offset;
	dir = rt::Vector3::Normalize(dir);
	return Ray(m_origin + offset, dir);
}
