#include "Metal.h"

Metal::Metal() {
	m_albedo = Vector3D(0.8f, 0.8f, 0.8f);
	m_roughness = 0.5f;
	m_ior = 1.5f;
}

Metal::Metal(const Vector3D& a, const float roughness, const float ior) {
	m_albedo = a;
	m_ior = ior;
	m_roughness = roughness;
}

Metal::~Metal() {
}

bool Metal::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	Vector3D reflected = Reflected(rayIn.GetDirection().UnitVector(), rec.GetNormal());

	Vector3D unitDir = rayIn.GetDirection().UnitVector();
	Vector3D unitDirInv = unitDir * -1.0f;

	float cosTheta = fminf(unitDirInv.DotProduct(rec.GetNormal()), 1.0f);
	float refracRatio = rec.GetFrontFace() ? (1.0f / m_ior) : m_ior;
	float fresnel = Schlick(cosTheta, refracRatio);

	Vector3D scatterDir = Vector3D::RandomInUnitSphere(32) * m_roughness;
	scatterDir = reflected + scatterDir;
	// Catch degenerate scatter direction
	if (scatterDir.NearZero()) scatterDir = rec.GetNormal();

	scattered = Ray(rec.GetPoint(), Vector3D::Lerp(reflected, scatterDir, 1.0f - fresnel));
	attentuation = m_albedo;

	return scattered.GetDirection().DotProduct(rec.GetNormal()) > 0.0f;
}
