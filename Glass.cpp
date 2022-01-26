#include "Glass.h"
#include "LinearFeedbackShift.h"
#include <mutex>

Glass::Glass() {
	m_albedo = Vector3D(1.0f, 1.0f, 1.0f);
	m_ior = 1.45f;
	m_roughness = 0.0f;
	m_transparent = true;
}

Glass::Glass(const Vector3D albedo, const float roughness, const float ior) {
	m_albedo = albedo;
	m_ior = ior;
	m_roughness = roughness;
	m_transparent = true;
}

bool Glass::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	float roughnessModified = m_roughness * m_roughness;
	float refracRatio = rec.GetFrontFace() ? (1.0f / m_ior) : m_ior;

	// fresnel
	Vector3D unitDir = rayIn.GetDirection().UnitVector();
	Vector3D unitDirInv = unitDir * -1.0f;

	Vector3D normal = rec.GetNormal().UnitVector();
	Vector3D incoming = rayIn.GetOrigin() - rec.GetPoint();
	incoming = incoming.UnitVector();

	Vector3D fresnelNormal = Vector3D::Lerp(normal, incoming, roughnessModified);
	fresnelNormal = fresnelNormal.UnitVector();

	float cosTheta = fminf(unitDirInv.DotProduct(fresnelNormal), 1.0f);
	float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
	float fresnel = Schlick(cosTheta, refracRatio);
	fresnel = std::clamp(fresnel, 0.0f, 1.0f);

	// Refraction
	//Vector3D refract = Refract(unitDir, rec.GetNormal(), refracRatio);

	// Reflection
	/*Vector3D reflect = Reflected(unitDir, rec.GetNormal());
	Vector3D reflectRough = reflect + (Vector3D::RandomInUnitSphere(32) * roughnessModified);*/
	unsigned int bitCount = 32;

	bool cannotRefract = refracRatio * sinTheta > 1.0f;
	bool fresnelRand = LinearFeedbackShift::RandFloat(bitCount) < fresnel;
	cannotRefract = cannotRefract || fresnelRand;

	Vector3D direction;
	if (cannotRefract) {
		direction = Reflected(unitDir, rec.GetNormal());
	}
	else {
		direction = Refract(unitDir, rec.GetNormal(), refracRatio);
	}

	Vector3D scatterDir;
	if (fresnelRand) {
		scatterDir = direction;
	}
	else {
		scatterDir = direction + (Vector3D::RandomInUnitSphere(bitCount) * roughnessModified);
	}


	attentuation = m_albedo;
	scattered = Ray(rec.GetPoint(), scatterDir);

	return true;
}
