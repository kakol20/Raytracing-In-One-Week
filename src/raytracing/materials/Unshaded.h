#ifndef UNSHADED_H
#define UNSHADED_H

#include "Material.h"

class Unshaded : public Material {
public:
	Unshaded() : Unshaded(Vector3D(1)) {};
	Unshaded(const Vector3D& albedo);
	virtual ~Unshaded();

	virtual void Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission);
};

#endif // !UNSHADED_H
