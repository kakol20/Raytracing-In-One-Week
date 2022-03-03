#include "Random.h"

#include "Dielectric.h"

Dielectric::Dielectric(const Vector3D& albedo, const float roughness, const float ior) {
	m_albedo = albedo;
	m_roughness = roughness;
	m_ior = ior;
}

bool Dielectric::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	// ----- NORMAL -----
	Vector3D unitDir = rayIn.GetDir();
	Vector3D incoming = unitDir * -1.0f;

	Vector3D normal = rec.GetNormal();

	normal.Normalize();
	incoming.Normalize();

	// ----- FRESNEL -----
	float sqrRoughness = m_roughness/* * m_roughness*/;

	bool roughnessRand = Random::RandFloat() <= sqrRoughness;

	Vector3D fresnelNormal = roughnessRand ? incoming : normal;

	float refractionRatio = rec.GetFrontFace() ? 1.f / m_ior : m_ior;
	float fresnel = Fresnel(incoming, fresnelNormal, refractionRatio);

	bool fresnelRand = Random::RandFloat() <= fresnel;

	attentuation = Vector3D::Lerp(m_albedo, Vector3D(1.f, 1.f, 1.f), fresnel);
	//attentuation = m_albedo;
	//attentuation = Vector3D(fresnel);

	Vector3D scatterDir;
	if (fresnelRand) {
		Vector3D reflect = Reflect(unitDir, normal);
		scatterDir = reflect;
		scatterDir += Vector3D::RandomInUnitSphere() * sqrRoughness;

		if (scatterDir.NearZero()) scatterDir = reflect;
		scatterDir.Normalize();
	}
	else {
		scatterDir = Vector3D::RandomInHemisphere(normal);
		if (scatterDir.NearZero()) scatterDir = normal;
		scatterDir.Normalize();
	}

	scattered = Ray(rec.GetPoint(), scatterDir);
	return true;
}
