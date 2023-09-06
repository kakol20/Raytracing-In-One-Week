#include "Dielectric.h"

Dielectric::Dielectric(const Vector3D& albedo, const Float roughness, const Float ior) {
	m_albedo = albedo;
	m_roughness = Float::Pow(roughness, 2.2);
	m_ior = ior;
}

void Dielectric::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission) {
	//normal = rec.GetNormal();
	//const Vector3D incoming = -rayIn.GetDir();
	//const Vector3D dir = rayIn.GetDir();

	//Float roughnessRand = Random::RandomFloat();


	//Float fresnelRand = Random::RandomFloat();

	//// diffuse part

	//Vector3D diffuseCol = m_albedo;
	//Vector3D diffuseScatter = Vector3D::RandomInUnitSphere() + normal;
	//diffuseScatter.Normalize();

	//// white glossy part
	//
	//Vector3D whiteGlossCol = Vector3D::One;
	//Vector3D whiteGlossScatter = Vector3D::Reflect(dir, normal) + ((Vector3D::RandomInUnitSphere() + normal) * m_roughness);

	//// mix them together
	//
	//Vector3D totalColor = Vector3D::RandomMix(diffuseCol, whiteGlossCol, fresnel, fresnelRand);
	//Vector3D totalScatter = Vector3D::RandomMix(diffuseScatter, whiteGlossScatter, fresnel, fresnelRand);
	//totalScatter.Normalize();

	//scattered = Ray(rec.GetPoint(), totalScatter);
	//attentuation = totalColor;
	//absorb = false;
	//transparent = false;
	//emission = false;

	//Vector3D unitDir = rayIn.GetDir();

	Vector3D incoming = -rayIn.GetDir();

	normal = rec.GetNormal();

	// ----- FRESNEL -----
	
	//const Float refractionRatio = rec.GetFrontFace() ? 1 / m_ior : m_ior;
	//Vector3D fresnelNormal = Vector3D::RandomMix(normal, incoming, m_roughness, roughnessRand);
	//Float fresnel = Fresnel(incoming, fresnelNormal, refractionRatio);

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
