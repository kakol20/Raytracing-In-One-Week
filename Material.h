#pragma once
#include "Ray.h"
#include "Object.h"
#include "Vector3D.h"

class Material {
public:
	Material() {};
	~Material() {};

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) = 0;
};