#pragma once

#include "Vector3D.h"

class Ray {
public:
	Ray();
	Ray(const Vector3D& orig, const Vector3D& dir);

	//Vector3D& operator=(const Vector3D& copyVector);
	Ray& operator=(const Ray& copyRay);

	const Vector3D At(const float t);

	~Ray();

public: // getters
	Vector3D GetDirection() { return m_dir; }
	Vector3D GetOrigin() { return m_orig; }

private:
	Vector3D m_orig;
	Vector3D m_dir;
};
