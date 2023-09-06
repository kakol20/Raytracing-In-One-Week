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
	//normal = rec.GetNormal();
	//const Vector3D incoming = -rayIn.GetDir();
	//const Vector3D dir = rayIn.GetDir();
	//const Float refractionRatio = rec.GetFrontFace() ? 1 / m_ior : m_ior;

	//Float roughnessRand = Random::RandomFloat();
	//Vector3D fresnelNormal = Vector3D::RandomMix(normal, incoming, m_roughness, roughnessRand);

	//Float fresnel = Fresnel(incoming, fresnelNormal, refractionRatio) - Fresnel(incoming, incoming, refractionRatio);
	//fresnel = Float::Clamp(fresnel, 0, 1);
	//Float fresnelRand = Random::RandomFloat();

	//// metallic part

	//Vector3D metalCol = m_albedo;
	//Vector3D metalScatter = Vector3D::Reflect(dir, normal) + ((Vector3D::RandomInUnitSphere() + normal) * m_roughness);

	//// glossy part

	//Float H, S, V;
	//ColorTools::RGBtoHSV(m_albedo, H, S, V);
	//S = Float::Clamp(S - 0.25, 0, 1);

	//Vector3D glossyCol = ColorTools::HSVToRGB(H, S, V);
	//Vector3D glossyScatter = Vector3D::Reflect(dir, normal);

	//// mix them together

	//Vector3D totalColor = Vector3D::RandomMix(metalCol, glossyCol, fresnel, fresnelRand);
	//Vector3D totalScatter = Vector3D::RandomMix(metalScatter, glossyScatter, fresnel, fresnelRand);
	//totalScatter.Normalize();

	//scattered = Ray(rec.GetPoint(), totalScatter);
	//attentuation = totalColor;
	//absorb = false;
	//transparent = false;
	//emission = false;

	//Vector3D unitDir = rayIn.GetDir();
	normal = rec.GetNormal();

	attentuation = m_albedo;

	absorb = true;
	emission = false;
	transparent = false;
}