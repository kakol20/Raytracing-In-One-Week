#pragma once
#include <algorithm>

#include "HitRec.h"
#include "Ray.h"
#include "Vector3D.h"

class Material {
public:
	Material() { m_ior = 1.5f; m_roughness = 0.5f; };
	virtual ~Material() {};

	virtual bool Emission(HitRec& rec, Vector3D& emission) = 0;
	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) = 0;
	virtual Vector3D GetNormal(HitRec& rec) = 0;

public:
	virtual bool IsTransmissive(Vector3D uv = Vector3D()) = 0;
	virtual float GetRoughness(Vector3D uv = Vector3D()) = 0;
	virtual Vector3D GetAlbedo(Vector3D uv = Vector3D()) = 0;

protected:
	float m_ior;
	float m_roughness;
	Vector3D m_albedo;

protected:
	const float Fresnel(Vector3D dir, Vector3D normal, float refIndex) {
		float cosTheta = Vector3D::DotProduct(dir, normal);
		cosTheta = std::clamp(cosTheta, 0.f, 1.f);

		/*float r0 = (1.f - refIndex) / (1.f + refIndex);
		r0 = r0 * r0;
		return r0 + (1.f - r0) * powf(1.f - cosTheta, 5.f);*/

		float r0 = (1.f - refIndex) / (1.f + refIndex);
		r0 *= r0;

		float fresnel = r0 + (1.f - r0) * powf(1.f - cosTheta, 5.f);

		return std::clamp(fresnel, 0.f, 1.f);
	}

	Vector3D Reflect(Vector3D v, Vector3D n) {
		Vector3D t = Vector3D::DotProduct(v, n);
		t *= 2.f;
		t *= n;
		return v - t;
	}

	Vector3D Refract(Vector3D v, Vector3D n, float refractionRatio) {
		Vector3D vInv = -v;

		float cosTheta = (std::min)(Vector3D::DotProduct(vInv, n), 1.f);
		Vector3D rOutPerp = (v + (n * cosTheta)) * refractionRatio;
		Vector3D rOutPara = n * -sqrtf(abs(1.f - rOutPerp.SqrMagnitude()));

		float sinTheta = sqrtf(1.f - cosTheta * cosTheta);

		bool cannotRefract = refractionRatio * sinTheta > 1.f;

		return cannotRefract ? Reflect(v, n) : rOutPerp + rOutPara;
	}
};