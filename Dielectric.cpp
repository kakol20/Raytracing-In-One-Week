#include <algorithm>

#include "LinearFeedbackShift.h"

#include "Dielectric.h"

Dielectric::Dielectric() {
	m_albedo = Vector3D(0.8f, 0.8f, 0.8f);
	m_roughness = 0.5f;
	m_ior = 1.5f;

	m_specular = (m_ior - 1) / (m_ior + 1);
	m_specular = (m_specular * m_specular) / 0.08f;
}

Dielectric::Dielectric(const Vector3D& a, const float roughness, const float ior) {
	m_albedo = a;
	m_roughness = roughness;
	m_ior = ior;

	m_specular = (m_ior - 1) / (m_ior + 1);
	m_specular = (m_specular * m_specular) / 0.08f;
}

Dielectric::~Dielectric() {
}

bool Dielectric::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	float roughnessModified = m_roughness * m_roughness;

	// fresnel
	Vector3D unitDir = rayIn.GetDirection().UnitVector();
	Vector3D unitDirInv = unitDir * -1.0f;

	Vector3D normal = rec.GetNormal().UnitVector();
	Vector3D incoming = rayIn.GetOrigin() - rec.GetPoint();
	incoming = incoming.UnitVector();

	Vector3D fresnelNormal = Vector3D::Lerp(normal, incoming, roughnessModified);
	fresnelNormal = fresnelNormal.UnitVector();

	float cosTheta = fminf(unitDirInv.DotProduct(fresnelNormal), 1.0f);
	float refracRatio = rec.GetFrontFace() ? (1.0f / m_ior) : m_ior;
	float fresnel = Schlick(cosTheta, refracRatio);
	fresnel = std::clamp(fresnel, 0.0f, 1.0f);

	// diffuse
	//Vector3D diffuse = Vector3D::RandomInHemisphere(rec.GetNormal(), 32);

	// glossy
	Vector3D white(0.8f, 0.8f, 0.8f);
	Vector3D glossy = Reflected(unitDir, rec.GetNormal());

	// mix diffuse and glossy
	//Vector3D albedo = Vector3D::Lerp(m_albedo, white, fresnel);
	//Vector3D scatterDir = Vector3D::Lerp(glossyRough, diffuse, fresnel);
	unsigned int bitCount = 32;
	bool randFresnel = LinearFeedbackShift::RandFloat(bitCount) < fresnel;

	Vector3D albedo;
	Vector3D scatterDir;
	if (randFresnel) {
		albedo = white;
		scatterDir = glossy + (Vector3D::RandomUnitVector(bitCount) * roughnessModified);
	}
	else {
		albedo = m_albedo;
		scatterDir = Vector3D::RandomInHemisphere(rec.GetNormal(), bitCount);
	}

	// apply
	attentuation = albedo;
	scattered = Ray(rec.GetPoint(), scatterDir);

	return true;
}