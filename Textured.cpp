#include "Random.h"

#include "Textured.h"

Textured::Textured(Image* albedo, Image* roughnessMetalEmission, Image* normal, const float ior, const Vector3D offset, const float emissionStrength) {
	m_albedoTexture = albedo;
	m_rmeTexture = roughnessMetalEmission;
	m_normalTexture = normal;
	m_ior = ior;
	m_uvOffset = offset;
	m_emissionStrength = emissionStrength;
}

Textured::~Textured() {
	m_albedoTexture = nullptr;
	m_rmeTexture = nullptr;
	m_normalTexture = nullptr;
}

bool Textured::Emission(HitRec& rec, Vector3D& emission) {
	// ----- ROUGHNESS, METALNESS & EMISSION -----
	Vector3D uv = (rec.GetUV() + m_uvOffset) * Vector3D(m_rmeTexture->GetWidth() - 1.f, m_rmeTexture->GetHeight() - 1.f, 0.f);

	float roughness, metalness, l_emission;
	m_rmeTexture->BiLerp(uv.GetX(), uv.GetY(), roughness, metalness, l_emission);
	//roughness /= 255.f;
	//metalness /= 255.f;
	l_emission /= 255.f;

	// ----- ALBEDO -----
	uv = (rec.GetUV() + m_uvOffset) * Vector3D(m_albedoTexture->GetWidth() - 1.f, m_albedoTexture->GetHeight() - 1.f, 0.f);
	float r, g, b;
	m_albedoTexture->BiLerp(uv.GetX(), uv.GetY(), r, g, b);
	Vector3D albedo(r, g, b);
	albedo /= 255.f;

	bool emissionRand = Random::RandFloat() <= l_emission;

	if (emissionRand) {
		emission = albedo * m_emissionStrength;
		return true;
	}
	else {
		return false;
	}
}

bool Textured::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	// ----- ROUGHNESS, METALNESS & EMISSION -----
	Vector3D uv = (rec.GetUV() + m_uvOffset) * Vector3D(m_rmeTexture->GetWidth() - 1.f, m_rmeTexture->GetHeight() - 1.f, 0.f);

	float roughness, metalness, emission;
	if (m_rmeTexture != nullptr) {
		m_rmeTexture->BiLerp(uv.GetX(), uv.GetY(), roughness, metalness, emission);
		roughness /= 255.f;
		metalness /= 255.f;
		emission /= 255.f;
	}
	else {
		roughness = 1.f;
		metalness = 0.f;
		emission = 0.f;
	}

	// ----- ALBEDO -----
	uv = (rec.GetUV() + m_uvOffset) * Vector3D(m_albedoTexture->GetWidth() - 1.f, m_albedoTexture->GetHeight() - 1.f, 0.f);
	float r, g, b;
	m_albedoTexture->BiLerp(uv.GetX(), uv.GetY(), r, g, b);

	r /= 255.f;
	g /= 255.f;
	b /= 255.f;
	Vector3D albedo(r, g, b);

	bool emissionRand = Random::RandFloat() < emission;
	if (emissionRand) {
		// ----- EMISSIVE -----
		attentuation = Vector3D();
		return false;
	}
	else {
		// ----- NORMAL -----
		Vector3D unitDir = rayIn.GetDir();
		Vector3D incoming = unitDir * -1.0;

		Vector3D normal = rec.GetNormal();

		// ----- FRESNEL -----
		float sqrRoughness = roughness * roughness;

		bool roughnessRand = Random::RandFloat() < sqrRoughness;

		Vector3D fresnelNormal = roughnessRand ? incoming : normal;

		float refractionRatio = rec.GetFrontFace() ? 1.f / m_ior : m_ior;
		float fresnel = Fresnel(incoming, fresnelNormal, refractionRatio);

		bool metalnessRand = Random::RandFloat() < metalness;

		bool fresnelRand = Random::RandFloat() < fresnel;

		if (metalnessRand) {
			// ----- METAL-----
			float incomingFresnel = fresnel - Fresnel(incoming, incoming, refractionRatio);

			float facing = Vector3D::DotProduct(incoming, normal);
			facing = std::clamp(facing, 0.f, 1.f);

			// color
			attentuation = Vector3D::Lerp(albedo, Vector3D(1.f, 1.f, 1.f), incomingFresnel);
			attentuation = Vector3D::Lerp(Vector3D(sqrtf(r), sqrtf(g), sqrtf(b)), attentuation, facing);

			// scatter
			Vector3D reflect = Reflect(unitDir, normal);

			Vector3D glossy = reflect + (Vector3D::RandomInUnitSphere() * sqrRoughness);
			if (glossy.NearZero()) glossy = reflect;
			glossy.Normalize();

			Vector3D scatterDir = fresnelRand ? reflect : glossy;

			scattered = Ray(rec.GetPoint(), scatterDir);
		}
		else {
			// ----- DIELECTRIC -----
			attentuation = Vector3D::Lerp(albedo, Vector3D(1.f, 1.f, 1.f), fresnel);

			Vector3D scatterDir;
			if (fresnelRand) {
				scatterDir = Reflect(unitDir, normal);
				scatterDir += Vector3D::RandomInUnitSphere() * sqrRoughness;

				if (scatterDir.NearZero()) scatterDir = Reflect(unitDir, normal);
				scatterDir.Normalize();
			}
			else {
				scatterDir = Vector3D::RandomInHemisphere(normal);
				if (scatterDir.NearZero()) scatterDir = normal;
				scatterDir.Normalize();
			}

			scattered = Ray(rec.GetPoint(), scatterDir);
		}
		return true;
	}
}

float Textured::GetRoughness(Vector3D uv) {
	// ----- ROUGHNESS, METALNESS & EMISSION -----
	Vector3D l_uv = (uv + m_uvOffset) * Vector3D(m_rmeTexture->GetWidth() - 1.f, m_rmeTexture->GetHeight() - 1.f, 0.f);

	float roughness, metalness, l_emission;
	m_rmeTexture->BiLerp(l_uv.GetX(), l_uv.GetY(), roughness, metalness, l_emission);
	roughness /= 255.f;
	//metalness /= 255.f;
	//l_emission /= 255.f;

	return roughness;
}

Vector3D Textured::GetAlbedo(Vector3D uv) {
	// ----- ALBEDO -----
	Vector3D l_uv = (uv + m_uvOffset) * Vector3D(m_albedoTexture->GetWidth() - 1.f, m_albedoTexture->GetHeight() - 1.f, 0.f);
	float r, g, b;
	m_albedoTexture->BiLerp(l_uv.GetX(), l_uv.GetY(), r, g, b);

	r /= 255.f;
	g /= 255.f;
	b /= 255.f;
	//Vector3D albedo(r, g, b);
	return Vector3D(r, g, b);
}