#pragma once
#include "Ray.h"
#include "HitRec.h"

class Material {
public:
	Material() {};
	~Material() {};

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) = 0;

private:

protected:
	Vector3D m_albedo;

	Vector3D Reflected(Vector3D v, Vector3D n) {
		Vector3D temp = n * v.DotProduct(n);
		temp *= 2.0f;
		return v - temp;
	}
};