#ifndef DIFFUSE_H
#define DIFFUSE_H

#include "Material.h"

class Diffuse : public Material {
public:
	Diffuse() : Diffuse(Vector3D::One) {};
	Diffuse(const Vector3D& albedo);
	virtual ~Diffuse() {};

	virtual void Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, bool& absorb, bool& transparent, bool& emission);
};

#endif // DIFFUSE_H
