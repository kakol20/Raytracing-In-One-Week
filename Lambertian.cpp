#include "Lambertian.h"

Lambertian::Lambertian() {
	m_albedo = Vector3D();
}

Lambertian::Lambertian(const Vector3D& a) {
	m_albedo = a;
}

Lambertian::~Lambertian() {
}

bool Lambertian::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	Vector3D scatterDir = rec.GetNormal() + Vector3D::RandomUnitVector();
	scattered = Ray(rec.GetPoint(), scatterDir);
	attentuation = m_albedo;
	return true;
}
