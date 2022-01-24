#include <cmath>

#include "Lambertian.h"
#include "LinearFeedbackShift.h"

Lambertian::Lambertian() {
	m_albedo = Vector3D();
}

Lambertian::Lambertian(const Vector3D& a, const float ior) {
	m_albedo = a;
	m_ior = ior;
	m_roughness = 1.0f;
	m_transparent = false;
}

Lambertian::~Lambertian() {
}

bool Lambertian::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	Vector3D unitDir = rayIn.GetDirection().UnitVector();
	//Vector3D unitDirInv = unitDir * -1.0f;
	//float cosTheta = fminf(unitDirInv.DotProduct(rec.GetNormal()), 1.0f);
	//float refracRatio = rec.GetFrontFace() ? (1.0f / m_ior) : m_ior;

	//float fresnel = Schlick(cosTheta, refracRatio);

	//float fresnelRoughness = std::lerp(fresnel, 1.0f, m_roughness);
	//fresnelRoughness = std::lerp(0.0f, 0.9f, fresnelRoughness);

	//Vector3D scatterDir = Vector3D::Lerp(reflect, refract, fresnelRoughness);
	//Vector3D scatterDir = rec.GetNormal() + (Vector3D::RandomUnitVector(32) /* * m_roughness*/);
	Vector3D original = Reflected(unitDir, rec.GetNormal());
	Vector3D scatterDir = Vector3D::RandomInHemisphere(rec.GetNormal(), 32);

	//scatterDir = Vector3D::Lerp(original, scatterDir, fresnelRoughness);

	// Catch degenerate scatter direction
	if (scatterDir.NearZero()) scatterDir = original;

	//scatterDir = scatterDir.UnitVector();

	scattered = Ray(rec.GetPoint(), scatterDir);
	attentuation = m_albedo;

	//if (LinearFeedbackShift::RandFloat(32) > fresnelRoughness) return false;

	return true;
}
