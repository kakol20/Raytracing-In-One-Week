#pragma once
#include "Material.h"

class Lambertian : public Material {
public:
	Lambertian();
	Lambertian(const Vector3D& a);
	~Lambertian();

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);

private:
};