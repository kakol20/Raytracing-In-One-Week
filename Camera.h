#pragma once
#include "Vector3D.h"
#include "Ray.h"

class Camera {
public:
	Camera();
	Camera(const float aspectRatio, const float vHeight, const float vWidth, const float focalLength);
	~Camera();

	Ray GetRay(const float u, const float v);

private:
	Vector3D m_horizontal;
	Vector3D m_lowerLeftCorner;
	Vector3D m_origin;
	Vector3D m_vertical;
};