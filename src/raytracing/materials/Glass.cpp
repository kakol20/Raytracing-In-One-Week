#include "../../misc/Random.h"

#include "Glass.h"

Glass::Glass(const Vector3D& albedo, const Float roughness, const Float ior) {
	m_albedo = albedo;
	m_roughness = roughness;
	m_ior = ior;
}

void Glass::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission) {
	// ----- NORMAL -----

	Vector3D incoming = -rayIn.GetDir();

	normal = rec.GetNormal();

	// ----- FRESNEL -----

	Float roughnessRand = Random::RandomFloat();

	//Vector3D fresnelNormal = roughnessRand ? incoming : normal;
	Vector3D fresnelNormal = Vector3D::RandomMix(normal, incoming, m_roughness, roughnessRand);

	Float refractionRatio = rec.GetFrontFace() ? 1 / m_ior : m_ior;
	Float fresnel = Fresnel(incoming, fresnelNormal, refractionRatio);

	// ----- MAIN VECTORS -----

	Vector3D refract = Vector3D::Refract(rayIn.GetDir(), normal, refractionRatio);

	Vector3D refractRough = refract + ((Vector3D::RandomInUnitSphere() + normal) * m_roughness);
	if (refractRough.NearZero()) refractRough = refract;
	//refractRough.Normalize();

	Vector3D reflect = Vector3D::Reflect(rayIn.GetDir(), normal);

	// ----- APPLY MATERIAL -----

	Float fresnelRand = Random::RandomFloat();

	//Vector3D scatterDir = fresnelRand ? reflect : refractRough;
	Vector3D scatterDir = Vector3D::RandomMix(refractRough, reflect, fresnel, fresnelRand);
	scatterDir.Normalize();

	//attentuation = Vector3D::Lerp(m_albedo, Vector3D::One, fresnel);
	attentuation = Vector3D::RandomMix(m_albedo, Vector3D::One, fresnel, fresnelRand);

	scattered = Ray(rec.GetPoint(), scatterDir);

	absorb = false;
	transparent = false;
	emission = false;
}
