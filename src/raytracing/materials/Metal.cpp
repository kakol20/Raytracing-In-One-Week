#include "../../misc/Random.h"
#include "../../misc/ColorTools.h"

#include "Metal.h"

Metal::Metal(const Vector3D& albedo, const Float roughness, const Float ior) {
	m_albedo = albedo;
	m_ior = ior;
	m_roughness = roughness * roughness;

	m_albedo = Vector3D::Clamp(m_albedo, Vector3D::Zero, Vector3D::One);
}

void Metal::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission) {
	normal = rec.GetNormal();
	Vector3D incoming = -rayIn.GetDir();

	// ----- FRESNEL -----

	Float roughnessRand = Random::RandomFloat();
	Vector3D fresnelNormal = Vector3D::RandomMix(normal, incoming, m_roughness, roughnessRand);

	Float fresnel = Fresnel(incoming, fresnelNormal, rec.GetFrontFace() ? 1 / m_ior : m_ior);
	fresnel *= fresnel;
	Float fresnelRand = Random::RandomFloat();

	// ----- METAL PART -----

	Vector3D metalScatter = Vector3D::Reflect(rayIn.GetDir(), normal) + (Vector3D::RandomInUnitSphere() * m_roughness);
	Vector3D metalCol = m_albedo;

	// ----- GLOSSY PART -----

	Vector3D glossyScatter = Vector3D::Reflect(rayIn.GetDir(), normal);

	Float H, S, V;
	ColorTools::RGBtoHSV(m_albedo, H, S, V);
	S -= 0.3333;
	Vector3D glossyCol = ColorTools::HSVToRGB(H, S, V);

	// ----- MIX -----

	Vector3D totalScatter = Vector3D::RandomMix(metalScatter, glossyScatter, fresnel, fresnelRand);
	totalScatter.Normalize();
	scattered = Ray(rec.GetPoint(), totalScatter);

	Vector3D totalCol = Vector3D::RandomMix(metalCol, glossyCol, fresnel, fresnelRand);
	attentuation = totalCol;

	absorb = false;
	emission = false;
	transparent = false;
}