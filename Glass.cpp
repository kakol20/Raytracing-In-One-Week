#include "Random.h"

#include "Glass.h"

Glass::Glass(const Vector3D& albedo, const float roughness, const float ior) {
	m_albedo = albedo;
	m_roughness = roughness;
	m_ior = ior;
}

bool Glass::Emission(HitRec& rec, Vector3D& emission) {
	return false;
}

bool Glass::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	// ----- NORMAL -----
	Vector3D unitDir = rayIn.GetDir();
	Vector3D incoming = unitDir * -1.0f;

	Vector3D normal = rec.GetNormal();

	// ----- FRESNEL -----
	float sqrRoughness = m_roughness * m_roughness;

	bool roughnessRand = Random::RandFloat() < sqrRoughness;

	Vector3D fresnelNormal = roughnessRand ? incoming : normal;

	float refractionRatio = rec.GetFrontFace() ? 1.f / m_ior : m_ior;
	float fresnel = Fresnel(incoming, fresnelNormal, refractionRatio);

	// ----- MAIN VECTORS -----
	Vector3D refract = Refract(unitDir, normal, refractionRatio);

	Vector3D refractRough = refract + (Vector3D::RandomInUnitSphere() * sqrRoughness);
	if (refractRough.NearZero()) refractRough = refract;
	refractRough.Normalize();

	Vector3D reflect = Reflect(unitDir, normal);

	// ----- APPLY MATERIAL -----
	bool fresnelRand = Random::RandFloat() < fresnel;

	Vector3D scatterDir = fresnelRand ? reflect : refractRough;

	attentuation = Vector3D::Lerp(m_albedo, Vector3D(1.f, 1.f, 1.f), fresnel);

	scattered = Ray(rec.GetPoint(), scatterDir);
	return true;
}
