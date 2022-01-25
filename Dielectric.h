#pragma once
#include "Material.h"

class Dielectric : public Material {
public:
	Dielectric();
	Dielectric(const Vector3D& a, const float roughness, const float ior);
	~Dielectric();

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);

private:
	float m_specular;
};