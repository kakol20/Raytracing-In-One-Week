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
	unsigned int bitCount = 32;

	Vector3D unitDir = rayIn.GetDirection().UnitVector();

	Vector3D original = Reflected(unitDir, rec.GetNormal());
	Vector3D scatterDir = Vector3D::RandomInHemisphere(rec.GetNormal(), bitCount);

	// Catch degenerate scatter direction
	if (scatterDir.NearZero()) scatterDir = original;


	scattered = Ray(rec.GetPoint(), scatterDir);
	attentuation = m_albedo;

	return true;
}
