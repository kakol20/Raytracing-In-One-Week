#include "ColorSpace.h"
#include "Random.h"

#include "Metal.h"

Metal::Metal(const Vector3D& albedo, const float roughness, const float ior) {
	m_albedo = albedo;
	m_roughness = roughness;
	m_ior = ior;

	m_edgeTint = ColorSpace::LinearTosRGB(albedo); // brightening color not converting to srgb
}

bool Metal::Emission(HitRec& rec, Vector3D& emission) {
	emission = Vector3D(0.f, 0.f, 0.f);
	return false;
}

bool Metal::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	// ----- NORMAL -----
	Vector3D unitDir = rayIn.GetDir();
	Vector3D incoming = unitDir * -1.0;

	Vector3D normal = rec.GetNormal();

	// ----- FRESNEL -----
	float sqrRoughness = m_roughness * m_roughness;

	bool roughnessRand = Random::RandFloat() < sqrRoughness;

	Vector3D fresnelNormal = roughnessRand ? incoming : normal;

	float refractionRatio = rec.GetFrontFace() ? 1.f / m_ior : m_ior;
	float fresnel = Fresnel(incoming, fresnelNormal, refractionRatio);
	float incomingFresnel = fresnel - Fresnel(incoming, incoming, refractionRatio);

	float facing = Vector3D::DotProduct(incoming, normal);
	facing = std::clamp(facing, 0.f, 1.f);

	// ----- MAIN VECTORS -----
	Vector3D reflect = Reflect(unitDir, normal);

	Vector3D glossy = reflect + (Vector3D::RandomInUnitSphere() * sqrRoughness);
	if (glossy.NearZero()) glossy = reflect;
	glossy.Normalize();

	// ----- APPLY MATERIAL -----
	bool fresnelRand = Random::RandFloat() <= incomingFresnel;

	Vector3D scatterDir = fresnelRand ? reflect : glossy;

	attentuation = Vector3D::Lerp(m_albedo, Vector3D(1.f, 1.f, 1.f), incomingFresnel);
	attentuation = Vector3D::Lerp(m_edgeTint, attentuation, facing);

	scattered = Ray(rec.GetPoint(), scatterDir);
	return true;
}
