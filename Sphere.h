#pragma once
#include "Object.h"
#include "Ray.h"

class Sphere : public Object {
public:
	Sphere(const Vector3D center, const float radius);

	virtual float Hit(Ray & ray);
	virtual Vector3D RayColor(Ray & ray);

private:
	float m_radius;
};