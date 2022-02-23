#pragma once

#include "Ray.h"
#include "HitRec.h"
#include "Vector3D.h"

class Object {
public:
	Object() {
		m_uvScale = Vector3D(1.f, 1.f, 1.f);
	};
	virtual ~Object() {};

	virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) = 0;

	virtual bool SphereIntersectGround(const Vector3D pos, const float radius) = 0;
	virtual bool SphereIntersectSphere(const Vector3D pos, const float radius) = 0;

	const Vector3D GetPos() const { return m_pos; };

protected:
	Vector3D m_pos;
	Material* m_mat = nullptr;
	Vector3D m_uvScale;
};