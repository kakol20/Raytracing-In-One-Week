#pragma once
#include "Material.h"

class Metal : public Material {
public:
	Metal();
	Metal(const Vector3D& a, const float roughness);
	~Metal();

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);

private:

	float m_roughness;
};