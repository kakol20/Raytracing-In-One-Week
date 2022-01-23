#pragma once
#include <cmath>

#include "Ray.h"
#include "HitRec.h"
#include "Light.h"

class Object;

class Material {
public:
	Material() {
		m_ior = 1.5f;
		m_roughness = 0.5f;
		m_transparent = false;
	};
	~Material() {};

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) = 0;

	const bool IsTransparent() { return m_transparent; };
	const Vector3D GetAlbedo() { return m_albedo; };
	const float GetRoughness() { return m_roughness; };

private:

protected:
	Vector3D m_albedo;
	float m_ior;
	float m_roughness;
	bool m_transparent;

	Vector3D Reflected(Vector3D v, Vector3D n) {
		Vector3D temp = n * v.DotProduct(n);
		temp *= 2.0f;
		return v - temp;
		//return v - (n * v.DotProduct(n) * 2);
	}

	Vector3D Refract(Vector3D v, Vector3D n, float n1OverN2) {
		Vector3D vInv = v * -1.0f;

		float cosTheta = fminf(vInv.DotProduct(n), 1.0f);
		Vector3D rOutPerp = (v + (n * cosTheta)) * n1OverN2;
		Vector3D rOutPara = n * -sqrtf(fabsf(1.0f - rOutPerp.SqrMagnitude()));

		return rOutPerp + rOutPara;
	}
	
	const float Schlick(float cosine, float refIndex) {
		// Use Schlick's approximation for reflectance.
		float r0 = (1.0f - refIndex) / (1.0f + refIndex);
		r0 *= r0;
		return r0 + (1.0f - r0) * powf(1.0f - cosine, 5.0f);
	}
};