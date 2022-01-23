#pragma once
#include "Material.h"

class Glass : public Material {
public:
	Glass();
	Glass(const Vector3D albedo, const float roughness, const float ior);

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);

private:	
};