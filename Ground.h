#pragma once
#include "Object.h"

class Ground : public Object {
public:
	Ground(const float height, Material* mat);

	virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec);
};
