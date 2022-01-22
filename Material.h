#pragma once
#include <cmath>

#include "Ray.h"
#include "HitRec.h"

class Material {
public:
	Material() {
		m_ior = 1.5f;
		m_roughness = 0.5f;
	};
	~Material() {};

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) = 0;

private:

protected:
	Vector3D m_albedo;
	float m_ior;
	float m_roughness;

	Vector3D Reflected(Vector3D v, Vector3D n) {
		/*Vector3D temp = n * v.DotProduct(n);
		temp *= 2.0f;
		return v - temp;*/
		return v - (n * (v.DotProduct(n) * 2));
	}

	bool Refract(Vector3D v, Vector3D n, float n1OverN2, Vector3D& refract) {
		Vector3D uv = v.UnitVector();
		float dt = uv.DotProduct(n);
		float discriminant = 1.0f - n1OverN2 * n1OverN2 * (1.0f - dt * dt);

		if (discriminant > 0) {
			refract = ((uv - (n * dt)) * n1OverN2) - (n * sqrtf(discriminant));
			return true;
		}

		return false;
	}
	
	const float Schlick(float cosine, float refIndex) {
		// Use Schlick's approximation for reflectance.
		float r0 = (1.0f - refIndex) / (1.0f + refIndex);
		r0 *= r0;
		return r0 + (1.0f - r0) * powf(1.0f - cosine, 5.0f);
	}
};