#include "Dielectric.h"

Dielectric::Dielectric(const Vector3D& albedo, const Float roughness, const Float ior) {
	m_albedo = albedo;
	m_roughness = roughness * roughness;
	m_ior = ior;
}

void Dielectric::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission) {
	Vector3D incoming = -rayIn.GetDir();

	normal = rec.GetNormal();

	// ----- FRESNEL -----

	Float roughnessRand = Random::RandomFloat();

	Vector3D fresnelNormal = Vector3D::RandomMix(normal, incoming, m_roughness, roughnessRand);

	Float fresnel = Fresnel(incoming, fresnelNormal, rec.GetFrontFace() ? 1 / m_ior : m_ior);
	Float fresnelRand = Random::RandomFloat();

	// ----- DIFFUSE PART -----

	Vector3D diffuseScatter = Vector3D::RandomInHemisphere(normal);
	Vector3D diffuseCol = m_albedo;

	// ----- GLOSSY PART -----

	Vector3D glossyScatter = Vector3D::Reflect(rayIn.GetDir(), normal) + (Vector3D::RandomInUnitSphere() * m_roughness);
	Vector3D glossyCol = Vector3D::One;

	// ----- MIX -----

	Vector3D totalScatter = Vector3D::RandomMix(diffuseScatter, glossyScatter, fresnel, fresnelRand);
	totalScatter.Normalize();
	Vector3D totalCol = Vector3D::RandomMix(diffuseCol, glossyCol, fresnel, fresnelRand);

	scattered = Ray(rec.GetPoint(), totalScatter);
	attentuation = totalCol;

	absorb = false;
	emission = false;
	transparent = false;
}
