#include "Metal.h"
#include "LinearFeedbackShift.h"

Metal::Metal() {
	m_albedo = Vector3D(0.8f, 0.8f, 0.8f);
	m_roughness = 0.5f;
	m_ior = 1.5f;
}

Metal::Metal(const Vector3D& a, const float roughness, const float ior) {
	m_albedo = a;
	m_ior = ior;
	m_roughness = roughness;
	m_transparent = false;
}

Metal::~Metal() {
}

bool Metal::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	float roughnessModified = m_roughness * m_roughness;

	Vector3D reflected = Reflected(rayIn.GetDirection().UnitVector(), rec.GetNormal());

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

	//float fresnelRoughness = std::lerp(fresnel, 1.0f, m_roughness);

	Vector3D scatterDir;

	/*scatterDir = Vector3D::Lerp(reflected, scatterDir, fresnelRoughness);*/
	unsigned int bitCount = 12;
	bool fresnelRand = LinearFeedbackShift::RandFloat(bitCount) < fresnel;

	if (fresnelRand) {
		scatterDir = reflected;
	}
	else {
		scatterDir = reflected + (Vector3D::RandomInUnitSphere(bitCount) * roughnessModified);
	}

	// Catch degenerate scatter direction
	if (scatterDir.NearZero()) scatterDir = reflected;

	scattered = Ray(rec.GetPoint(), scatterDir);
	attentuation = m_albedo;

	return true;
}
