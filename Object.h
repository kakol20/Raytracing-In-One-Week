#pragma once
#include "Ray.h"
#include "HitRec.h"
#include "Lambertian.h"

class Object {
public:
	Object() {};

	virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec & rec) = 0;
	//virtual Vector3D RayColor(Ray& ray) = 0;

	const Vector3D GetPos() { return m_pos; };

	~Object() {};

protected:
	Vector3D m_pos;
	Material* m_mat = nullptr;

};