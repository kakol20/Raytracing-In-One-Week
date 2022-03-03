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
	Vector3D normal = rec.GetNormal();

	Vector3D unitDir = rayIn.GetDir();
	if (rayIn.GetDir().NearZero()) unitDir = normal * -1.f;
	Vector3D incoming = unitDir * -1.0f;

	// ----- FRESNEL -----
	float sqrRoughness = m_roughness/* * m_roughness*/;

	bool roughnessRand = Random::RandFloat() < sqrRoughness;

	Vector3D fresnelNormal = roughnessRand ? incoming : normal;

	float refractionRatio = rec.GetFrontFace() ? 1.f / m_ior : m_ior;
	float fresnel = Fresnel(incoming, fresnelNormal, refractionRatio);
	float incomingFresnel = fresnel - Fresnel(incoming, incoming, refractionRatio);
	incomingFresnel = std::clamp(incomingFresnel, 0.f, 1.f);

	float facing = Vector3D::DotProduct(incoming, normal);
	//facing = std::clamp(facing, 0.f, 1.f);

	// ----- MAIN VECTORS -----
	Vector3D reflect = Reflect(unitDir, normal);
	//Vector3D reflect = Vector3D::RandomInHemisphere(normal);
	//reflect.Normalize();

	Vector3D glossy = reflect + (Vector3D::RandomInUnitSphere() * sqrRoughness);
	if (glossy.NearZero()) glossy = reflect;
	glossy.Normalize();

	// ----- APPLY MATERIAL -----
	bool fresnelRand = Random::RandFloat() < incomingFresnel;
	//bool fresnelRand = Random::RandFloat() < fresnel;

	Vector3D scatterDir = fresnelRand ? reflect : glossy;
	//Vector3D scatterDir = glossy;

	bool debug = false;
	if (debug) {
		if (attentuation.NearZero()) attentuation = m_albedo;

		float scatterDot = Vector3D::DotProduct(scatterDir, normal);

		attentuation = scatterDot < 0.f ? Vector3D(0.f, 1.f) : Vector3D(1.f, 0.f);
		scatterDir = Vector3D::RandomInHemisphere(normal);
	}
	else {
		attentuation = Vector3D::Lerp(m_albedo, Vector3D(1.f, 1.f, 1.f), incomingFresnel);
		attentuation = Vector3D::Lerp(m_edgeTint, attentuation, facing);
	}

	scatterDir.Normalize();

	scattered = Ray(rec.GetPoint(), scatterDir);
	return true;
}