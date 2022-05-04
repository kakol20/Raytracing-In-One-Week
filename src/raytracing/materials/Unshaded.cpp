#include "Unshaded.h"

Unshaded::Unshaded(const Vector3D& albedo) {
	m_albedo = albedo;
}

Unshaded::~Unshaded() {
}

void Unshaded::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, bool& absorb, bool& transparent, bool& emission) {
	transparent = false;
	absorb = true;
	emission = false;

	attentuation = m_albedo;
}
