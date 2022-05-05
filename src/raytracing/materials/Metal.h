#ifndef METAL_H
#define METAL_H

#include "Material.h"

class Metal : public Material {
public:
	Metal() : Metal(Vector3D::One, 0, 1.5) {};
	Metal(const Vector3D& albedo, const Float& roughness, const Float& ior);
	virtual ~Metal() {};

	virtual void Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission);
};
#endif // !METAL_H
