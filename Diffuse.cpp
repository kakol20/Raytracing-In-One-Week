#include "Diffuse.h"

Diffuse::Diffuse() : Diffuse(Vector3D(1.f, 1.f, 1.f)) {
}

Diffuse::Diffuse(const Vector3D& albedo) {
	m_ior = 1.5;
	m_roughness = 0.0;

	m_albedo = albedo;
}

Diffuse::~Diffuse() {
}

bool Diffuse::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	Vector3D unitDir = rayIn.GetDir();

	Vector3D scatterDir = Vector3D::RandomInHemisphere(rec.GetNormal());
	scatterDir.Normalize();

	attentuation = m_albedo;
	scattered = Ray(rec.GetPoint(), scatterDir);
	return true;
}
