#pragma once

#include "Vector3D.h"

class Ray {
public:
	Ray();
	Ray(const Vector3D& orig, const Vector3D& dir);
	~Ray() {};

	Ray& operator=(const Ray& copyRay);

	Vector3D At(const float t);

	Vector3D GetDir() { return m_dir; };
	void SetDir(const Vector3D dir) { m_dir = dir; };

	Vector3D GetOrig() { return m_orig; };
	void SetOrig(const Vector3D orig) { m_orig = orig; };

private:
	Vector3D m_dir;
	Vector3D m_orig;
};

