#include "Unshaded.h"

Unshaded::Unshaded(const Unshaded& other) {
	m_albedo = other.m_albedo;
}

Unshaded::Unshaded(const Vector3D& albedo) {
	m_albedo = albedo;
}

Unshaded::~Unshaded() {
}

void Unshaded::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission) {
	transparent = false;
	absorb = true;
	emission = true;
	normal = rec.GetNormal();

	attentuation = m_albedo;
}

Unshaded& Unshaded::operator=(const Unshaded& other) {
	// TODO: insert return statement here
	if (this == &other) return *this;
	m_albedo = other.m_albedo;
	return *this;
}
