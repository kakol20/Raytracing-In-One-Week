#include "Dielectric.h"

Dielectric::Dielectric(const Vector3D& albedo, const Float roughness, const Float ior) {
	m_albedo = albedo;
	m_roughness = roughness;
	m_ior = ior;
}

void Dielectric::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission) {
	normal = rec.GetNormal();
	const Vector3D incoming = rayIn.GetDir() * -1;

	Float fresnel = Fresnel(incoming, normal, m_ior);

	// diffuse part

	Vector3D diffuseCol = m_albedo;
	Vector3D diffuseScatter = Vector3D::RandomInUnitSphere() + normal;
	diffuseScatter.Normalize();

	Vector3D totalScatter = diffuseScatter;
	Vector3D totalColor = diffuseCol;

	scattered = Ray(rec.GetPoint(), totalScatter);
	attentuation = totalColor;
	absorb = false;
	transparent = false;
	emission = false;
}
