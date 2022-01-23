#include "Glass.h"
#include "LinearFeedbackShift.h"
#include <mutex>

Glass::Glass() {
	m_albedo = Vector3D(1.0f, 1.0f, 1.0f);
	m_ior = 1.45f;
	m_roughness = 0.0f;
}

Glass::Glass(const Vector3D albedo, const float roughness, const float ior) {
	m_albedo = albedo;
	m_ior = ior;
	m_roughness = roughness;
}

bool Glass::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	float refracRatio = rec.GetFrontFace() ? (1.0f / m_ior) : m_ior;

	Vector3D unitDir = rayIn.GetDirection().UnitVector();
	Vector3D unitDirInv = unitDir * -1.0f;

	// Total internal reflection
	float cosTheta = fminf(unitDirInv.DotProduct(rec.GetNormal()), 1.0f);
	float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);

	bool cannotRefract = refracRatio * sinTheta > 1.0f;
	float schlick = Schlick(cosTheta, refracRatio);

	Vector3D direction;

	if (cannotRefract || schlick > LinearFeedbackShift::RandFloat(32)) {
		direction = Reflected(unitDir, rec.GetNormal());
	}
	else {
		direction = Refract(unitDir, rec.GetNormal(), refracRatio);
	}

	Vector3D scatterDir = Vector3D::RandomInUnitSphere(32) * m_roughness;
	scatterDir = direction + scatterDir;
	// Catch degenerate scatter direction

	if (scatterDir.NearZero()) scatterDir = rec.GetNormal();

	scattered = Ray(rec.GetPoint(), Vector3D::Lerp(direction, scatterDir, schlick));
	attentuation = m_albedo;

	return true;
}
