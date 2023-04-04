#include "../../misc/Random.h"

#include "Metal.h"

Metal::Metal(const Vector3D& albedo, const Float roughness, const Float ior) {
	m_albedo = albedo;
	m_ior = ior;
	m_roughness = roughness;
}

void Metal::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission) {
	// ----- NORMAL -----
	normal = rec.GetNormal();

	Vector3D incoming = -rayIn.GetDir();

	// ----- FRESNEL -----

	bool roughnessRand = Random::RandomFloat() < m_roughness;

	Vector3D fresnelNormal = roughnessRand ? incoming : normal;

	Float refractionRatio = rec.GetFrontFace() ? 1 / m_ior : m_ior;
	Float fresnel = Fresnel(incoming, fresnelNormal, refractionRatio);
	Float incomingFresnel = fresnel - Fresnel(incoming, incoming, refractionRatio);
	incomingFresnel = Float::Clamp(incomingFresnel, 0, 1);

	// ----- MAIN VECTORS -----

	Vector3D reflect = Vector3D::Reflect(rayIn.GetDir(), normal);

	Vector3D glossy = reflect + ((Vector3D::RandomInUnitSphere() + normal) * m_roughness);
	if (glossy.NearZero()) glossy = reflect;
	glossy.Normalize();

	// ----- APPLY MATERIAL -----

	bool fresnelRand = Random::RandomFloat() < incomingFresnel;

	Vector3D scatterDir = fresnelRand ? reflect : glossy;

	attentuation = Vector3D::Lerp(m_albedo, Vector3D::One, incomingFresnel);

	scatterDir.Normalize();

	scattered = Ray(rec.GetPoint(), scatterDir);

	if (Vector3D::DotProduct(normal, scatterDir) < Float::NearZero) {
		absorb = true;
	}
	else {
		absorb = false;
	}
	transparent = false;
	emission = false;
}