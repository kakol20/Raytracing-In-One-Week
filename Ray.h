#pragma once

#include "Vector3D.h"

class Ray {
public:
	Ray();
	Ray(const Vector3D& orig, const Vector3D& dir);

	const Vector3D At(const float t);

	~Ray();

public: // getters
	const Vector3D GetDirection() { return m_dir; }
	const Vector3D GetOrigin() { return m_orig; }

private:
	Vector3D m_orig;
	Vector3D m_dir;
};

