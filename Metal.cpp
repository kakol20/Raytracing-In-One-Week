#include "Metal.h"

Metal::Metal() {
	m_albedo = Vector3D(0.8f, 0.8f, 0.8f);
	m_roughness = 0.5f;
}

Metal::Metal(const Vector3D& a, const float roughness) {
	m_albedo = a;
	m_roughness = roughness;
}

Metal::~Metal() {
}

bool Metal::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	Vector3D reflected = Reflected(rayIn.GetDirection().UnitVector(), rec.GetNormal());
	scattered = Ray(rec.GetPoint(), reflected + (Vector3D::RandomInUnitSphere() * m_roughness));
	attentuation = m_albedo;

	return scattered.GetDirection().DotProduct(rec.GetNormal()) > 0.0f;
}

Vector3D Metal::Reflected(Vector3D v, Vector3D n) {
	Vector3D temp = n * v.DotProduct(n);
	temp *= 2.0f;
	return v - temp;
}
