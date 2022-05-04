#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "../../maths/Vector3D.h"
#include "../HitRec.h"
#include "../Ray.h"

class Material {
public:
	Material() {
		m_ior = 1.5;
		m_roughness = 0.5;
	}
	virtual ~Material() {};

	virtual Vector3D GetNormal(HitRec& rec) {
		return rec.GetNormal();
	}

	virtual void Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, bool& absorb, bool& transparent, bool& emission) = 0;

protected:
	Float Fresnel(const Vector3D& dir, const Vector3D& normal, const Float& refIndex) const {
		Float cosTheta = Vector3D::DotProduct(dir, normal);
		cosTheta = Float::Clamp(cosTheta, 0, 1);

		Float r0 = (1 - refIndex) / (1 + refIndex);
		r0 *= r0;

		Float pow5 = Float::Pow(1 - cosTheta, 5);

		Float fresnel = r0 + ((1 - r0) * pow5);
		return Float::Clamp(fresnel, 0, 1);
	}

protected:
	Float m_ior;
	Float m_roughness;
	Vector3D m_albedo;
};
#endif // !MATERIAL_HPP