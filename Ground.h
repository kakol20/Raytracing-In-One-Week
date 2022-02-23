#pragma once
#include "Object.h"
class Ground :
	public Object {
public:
	Ground();
	Ground(const float height, Material* mat, const Vector3D uvScale = Vector3D(1.f, 1.f, 1.f));
	virtual ~Ground();

	virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

	virtual bool SphereIntersectGround(const Vector3D pos, const float radius);
	virtual bool SphereIntersectSphere(const Vector3D pos, const float radius);
};

