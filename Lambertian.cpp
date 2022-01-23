#include "Lambertian.h"

Lambertian::Lambertian() {
	m_albedo = Vector3D();
}

Lambertian::Lambertian(const Vector3D& a, const float ior) {
	m_albedo = a;
	m_ior = ior;
	m_roughness = 1.0f;
}

Lambertian::~Lambertian() {
}

bool Lambertian::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {

	Vector3D unitDir = rayIn.GetDirection().UnitVector();
	Vector3D unitDirInv = unitDir * -1.0f;
	float cosTheta = fminf(unitDirInv.DotProduct(rec.GetNormal()), 1.0f);

	float refracRatio = rec.GetFrontFace() ? (1.0f / m_ior) : m_ior;
	float schlick = Schlick(cosTheta, refracRatio);

	Vector3D scatterDir = Vector3D::RandomInUnitSphere(32) * m_roughness;
	scatterDir = rec.GetNormal() + scatterDir;

	// Catch degenerate scatter direction
	if (scatterDir.NearZero()) scatterDir = rec.GetNormal();

	scattered = Ray(rec.GetPoint(), Vector3D::Lerp(rec.GetNormal(), scatterDir, 1.0f - schlick));
	attentuation = m_albedo;

	return true;
}
