#pragma once
#include "Material.h"

class Metal : public Material {
public:
	Metal(const Vector3D& color);

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attenuation, Ray& scattered);

private:
	Vector3D Reflect(const Vector3D& v, const Vector3D& n);
};

