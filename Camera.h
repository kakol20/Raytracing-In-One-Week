#pragma once
#include "Ray.h"
#include "Vector3D.h"

class Camera {
public:
	Camera();
	Camera(const float aspectRatio, const float aperture, const float focusDist, const float vFOV, const Vector3D lookFrom, const Vector3D lookAt, const Vector3D up);
	~Camera();

	Camera& operator=(const Camera& copyCamera);

	Ray GetRay(const float s, const float t) const;

private:
	float m_lensRadius;
	Vector3D m_horizontal;
	Vector3D m_lowerLeftCorner;
	Vector3D m_origin;
	Vector3D m_u;
	Vector3D m_v;
	Vector3D m_vertical;
	Vector3D m_w;
};

