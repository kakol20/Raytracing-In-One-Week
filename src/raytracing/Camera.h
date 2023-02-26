#ifndef CAMERA_H
#define CAMERA_H

#include "../raytracing/Camera.h"
#include "Ray.h"

class Camera {
public:
	Camera() : Camera(1, 0, 10, 90, Vector3D(0), Vector3D::Forward, Vector3D::Up) {};
	Camera(const Camera& copyCamera);
	Camera(const Float& aspectRatio, const Float& blurStrength, const Float& focusDist, const Float& vFOV, const Vector3D& lookFrom, const Vector3D& lookAt, const Vector3D& up);
	~Camera() {};

	Camera& operator=(const Camera& copyCamera);

	Ray GetRay(const Float& s, const Float& t) const;

private:
	Float m_lensRadius;
	Vector3D m_horizontal;
	Vector3D m_lowerLeftCorner;
	Vector3D m_origin;
	Vector3D m_u;
	Vector3D m_v;
	Vector3D m_vertical;
	Vector3D m_w;
};
#endif // !CAMERA_H
