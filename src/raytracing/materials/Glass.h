#ifndef GLASS_H
#define GLASS_H

#include "Material.h"

class Glass : public Material {
public:
	Glass() : Glass(Vector3D::One, 0, 1.5) {};
	Glass(const Vector3D& albedo, const Float& roughness, const Float& ior);
	virtual ~Glass() {};

	virtual void Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission);
};
#endif // !GLASS_H
