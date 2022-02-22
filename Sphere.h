#pragma once
#include "Object.h"
class Sphere :
	public Object {
public:
	Sphere();
	Sphere(const Vector3D center, const float radius, Material* mat, const float uvScale = 1.f);
	virtual ~Sphere();

	virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

	virtual bool SphereIntersectGround(const Vector3D pos, const float radius);
	virtual bool SphereIntersectSphere(const Vector3D pos, const float radius);

private:
	float m_radius;

private:
	Vector3D CalculateTangent(HitRec& rec);
	Vector3D CalculateUV(const Vector3D point);
};

