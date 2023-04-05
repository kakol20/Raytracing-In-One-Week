#pragma once
#include "Material.h"

class Dielectric : public Material {
public:
	Dielectric() : Dielectric(Vector3D::One, 0, 1.5) {};
	Dielectric(const Vector3D& albedo, const Float roughness, const Float ior);
	virtual ~Dielectric() {};

	virtual void Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission);
};