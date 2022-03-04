#include "Emissive.h"

Emissive::Emissive() : Emissive(Vector3D(0.f, 0.f, 0.f), 1.f) {
}

Emissive::Emissive(const Vector3D& albedo, const float intensity) {
	m_ior = 1.5;
	m_roughness = 0.0;

	m_albedo = albedo;
	m_intensity = intensity;
}

Emissive::~Emissive() {
}

bool Emissive::Emission(HitRec& rec, Vector3D& emission) {
	emission = m_albedo * m_intensity;
	return true;
}

bool Emissive::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	attentuation = m_albedo;
	return false;
}
