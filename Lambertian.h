#pragma once
#include "Material.h"

class Lambertian : public Material {
public:
	Lambertian(const Vector3D& color);

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attenuation, Ray& scattered);

private:
};
