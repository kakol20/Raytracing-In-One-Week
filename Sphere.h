#pragma once
#include "Object.h"
#include "Ray.h"

class Sphere : public Object {
public:
	Sphere(const Vector3D center, const float radius);

	virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

private:
	float m_radius;
};