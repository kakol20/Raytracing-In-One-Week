#include "Diffuse.h"

Diffuse::Diffuse(const Vector3D& albedo) {
	m_albedo = albedo;
	m_roughness = 0;
	m_ior = 1.5;
}

void Diffuse::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, bool& absorb, bool& transparent, bool& emission) {
	Vector3D unitDir = rayIn.GetDir();

	Vector3D scatterDir = Vector3D::RandomInHemisphere(rec.GetNormal());
	scatterDir.Normalize();

	Float dotProduct = Vector3D::DotProduct(rec.GetNormal(), scatterDir);

	if (dotProduct < 0) scatterDir = unitDir * -1;

	attentuation = m_albedo;
	scattered = Ray(rec.GetPoint(), scatterDir);

	dotProduct = Vector3D::DotProduct(rec.GetNormal(), scatterDir);

	absorb = false;
	transparent = false;
	emission = false;
}