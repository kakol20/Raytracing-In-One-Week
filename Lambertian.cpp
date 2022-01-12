#include "Lambertian.h"

Lambertian::Lambertian(const Vector3D& color) {
	m_albedo = color;
}

bool Lambertian::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attenuation, Ray& scattered) {
	Vector3D scatterDir = rec.normal + Vector3D::RandomUnitVector();

	// Catch degenerate scatter direction
	if (scatterDir.NearZero()) scatterDir = rec.normal;

	scattered = Ray(rec.point, scatterDir);
	attenuation = m_albedo;
	return true;
}
