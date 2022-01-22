#pragma once
#include "Material.h"

class Glass : public Material {
public:
	Glass();
	Glass(const Vector3D albedo, const float ior, const float roughness);

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);

private:
	float m_ior;
	float m_roughness;

	const float Reflectance(float cosine, float refIndex);
	Vector3D Refract(const Vector3D uv, const Vector3D n, const float eoe);
};