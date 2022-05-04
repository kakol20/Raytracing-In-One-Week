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

Camera::Camera(const Float& aspectRatio, const Float& blurStrength, const Float& focusDist, const Float& vFOV, const Vector3D& lookFrom, const Vector3D& lookAt, const Vector3D& up) {
	const Float theta = vFOV * Float::ToRadians;
	const Float h = Float::Tan(theta / 2);
	const Float vHeight = 2 * h;
	const Float vWidth = aspectRatio * vHeight;

	// calculate position
	m_w = lookFrom - lookAt;
	m_w.Normalize();

	m_u = Vector3D::CrossProduct(up, m_w);
	m_u.Normalize();

	m_v = Vector3D::CrossProduct(m_w, m_u);
	//m_v.Normalize();

	m_origin = lookFrom;

	m_horizontal = m_u * vWidth * focusDist;
	m_vertical = m_v * vHeight * focusDist;

	m_lowerLeftCorner = m_origin - (m_horizontal / 2) - (m_vertical / 2) - (m_w * focusDist);

	m_lensRadius = blurStrength / 2;

#ifdef _DEBUG
	Float::FlOrDo l_theta = theta.ToFloat();
	Float::FlOrDo l_h = h.ToFloat();
	Float::FlOrDo l_vHeight = vHeight.ToFloat();
	Float::FlOrDo l_vWidth = vWidth.ToFloat();

	{
		bool temp = false;
	}
#endif // _DEBUG
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

	/*
		Float m_lensRadius;
		Vector3D m_horizontal;
		Vector3D m_lowerLeftCorner;
		Vector3D m_origin;
		Vector3D m_u;
		Vector3D m_v;
		Vector3D m_vertical;
		Vector3D m_w;
	*/

	return *this;
}

Ray Camera::GetRay(const Float& s, const Float& t) const {
	Vector3D rd = Vector3D::RandomInUnitDisk() * m_lensRadius;
	Vector3D offset = (m_u * rd.GetX()) + (m_v * rd.GetY());

	Vector3D dir = m_lowerLeftCorner + (m_horizontal * s) + (m_vertical * t) - m_origin - offset;
	dir.Normalize();
	return Ray(m_origin + offset, dir);
}