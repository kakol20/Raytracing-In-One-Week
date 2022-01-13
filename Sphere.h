#pragma once
#include "Object.h"

class Sphere : public Object {
public:
	Sphere(const Vector3D center, const float radius, Material* mat);

	virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

private:
	float m_radius;
};