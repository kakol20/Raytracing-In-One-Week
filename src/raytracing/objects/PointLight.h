#pragma once

#include "Object.h"

#include "../materials/Unshaded.h"

class PointLight : public Object {
public:
	PointLight(const Unshaded& mat, const Float radius = 1., const Vector3D& pos = Vector3D::Zero);
	virtual ~PointLight() {};

	virtual bool Hit(Ray& ray, const Float t_min, const Float t_max, HitRec& rec);

	virtual bool SphereIntersectGround(const Vector3D& pos, const Float radius) { return false; };
	virtual bool SphereIntersectSphere(const Vector3D& pos, const Float radius) { return false; };
private:
	Float m_radius;
	Unshaded m_light_mat;
};