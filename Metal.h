#pragma once
#include "Material.h"

class Metal : public Material {
public:
	Metal();
	Metal(const Vector3D& a);
	~Metal();

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);

private:
	Vector3D Reflected(Vector3D v, Vector3D n);
};
