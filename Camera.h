#pragma once
#include "Vector3D.h"
#include "Ray.h"

class Camera {
public:
	Camera();
	Camera(const float aspectRatio, const float vfov, Vector3D lookFrom, Vector3D lookAt, Vector3D vUp);
	~Camera();

	Ray GetRay(const float s, const float t);

private:
	const float Degrees2Radians(const float deg);

private:
	Vector3D m_horizontal;
	Vector3D m_lowerLeftCorner;
	Vector3D m_origin;
	Vector3D m_vertical;
};