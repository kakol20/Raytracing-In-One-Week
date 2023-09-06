#include "../../misc/Random.h"

#include "Glass.h"

Glass::Glass(const Vector3D& albedo, const Float roughness, const Float ior) {
	m_albedo = albedo;
	m_roughness = Float::Pow(roughness, 2.2);;
	m_ior = ior;
}

void Glass::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission) {
	//normal = rec.GetNormal();
	//const Vector3D incoming = -rayIn.GetDir();
	//const Vector3D dir = rayIn.GetDir();
	//const Float refractionRatio = rec.GetFrontFace() ? 1 / m_ior : m_ior;

	//Float roughnessRand = Random::RandomFloat();

	//Vector3D fresnelNormal = Vector3D::RandomMix(normal, incoming, m_roughness, roughnessRand);

	//const Float fresnel = Fresnel(incoming, fresnelNormal, refractionRatio);
	//const Float fresnelRand = Random::RandomFloat();

	//// refraction part

	//Vector3D refractCol = m_albedo;
	//Vector3D refractScatter = Vector3D::Refract(dir, normal, refractionRatio) + ((Vector3D::RandomInUnitSphere() + normal) * m_roughness);

	//// glossy part

	//Vector3D glossyCol = Vector3D::One;
	//Vector3D glossyScatter = Vector3D::Reflect(dir, normal) + ((Vector3D::RandomInUnitSphere() + normal) * m_roughness);

	//// mix them together

	//Vector3D totalColor = Vector3D::RandomMix(refractCol, glossyCol, fresnel, fresnelRand);
	//Vector3D totalScatter = Vector3D::RandomMix(refractScatter, glossyScatter, fresnel, fresnelRand);
	//totalScatter.Normalize();

	//scattered = Ray(rec.GetPoint(), totalScatter);
	//attentuation = totalColor;
	//absorb = false;
	//transparent = false;
	//emission = false;

	//Vector3D unitDir = rayIn.GetDir();
	normal = rec.GetNormal();
	attentuation = m_albedo;

	Vector3D scatterDir = Vector3D::RandomInHemisphere(normal);
	scatterDir.Normalize();
	scattered = Ray(rec.GetPoint(), scatterDir);

	absorb = false;
	emission = false;
	transparent = false;
}
