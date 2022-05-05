#include "../../misc/Random.h"

#include "Glass.h"

Glass::Glass(const Vector3D& albedo, const Float& roughness, const Float& ior) {
	m_albedo = albedo;
	m_roughness = roughness;
	m_ior = ior;
}

void Glass::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission) {
	// ----- NORMAL -----

	Vector3D unitDir = rayIn.GetDir();
	Vector3D incoming = -unitDir;

	normal = rec.GetNormal();

	// ----- FRESNEL -----

	Float sqrRoughness = m_roughness/* * m_roughness*/;

	bool roughnessRand = Random::RandomFloat() < sqrRoughness;

	Vector3D fresnelNormal = roughnessRand ? incoming : normal;

	Float refractionRatio = rec.GetFrontFace() ? 1 / m_ior : m_ior;
	Float fresnel = Fresnel(incoming, fresnelNormal, refractionRatio);

	// ----- MAIN VECTORS -----

	Vector3D refract = Vector3D::Refract(unitDir, normal, refractionRatio);

	Vector3D refractRough = refract + (Vector3D::RandomInUnitSphere() * sqrRoughness);
	if (refractRough.NearZero()) refractRough = refract;
	refractRough.Normalize();

	Vector3D reflect = Vector3D::Reflect(unitDir, normal);

	// ----- APPLY MATERIAL -----

	bool fresnelRand = Random::RandomFloat() < fresnel;

	Vector3D scatterDir = fresnelRand ? reflect : refractRough;

	attentuation = Vector3D::Lerp(m_albedo, Vector3D::One, fresnel);

	scattered = Ray(rec.GetPoint(), scatterDir);

	/*if (Vector3D::DotProduct(scatterDir, normal) < 0) {
		absorb = true;
	}
	else {
		absorb = false;
	}*/
	absorb = false;
	transparent = false;
	emission = false;
}
