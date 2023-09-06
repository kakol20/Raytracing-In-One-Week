#include "Diffuse.h"

Diffuse::Diffuse(const Vector3D& albedo) {
	m_albedo = albedo;
	m_roughness = 0;
	m_ior = 1.5;
}

void Diffuse::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered, Vector3D& normal, bool& absorb, bool& transparent, bool& emission) {
	//Vector3D unitDir = rayIn.GetDir();

	//normal = rec.GetNormal();

	////Vector3D scatterDir = Vector3D::RandomInHemisphere(normal);
	//Vector3D scatterDir = Vector3D::RandomInUnitSphere();
	//scatterDir += normal;
	//scatterDir.Normalize();

	//Float dotProduct = Vector3D::DotProduct(normal, scatterDir);

	//if (dotProduct < 0) scatterDir = unitDir * -1;

	//attentuation = m_albedo;
	//scattered = Ray(rec.GetPoint(), scatterDir);

	//dotProduct = Vector3D::DotProduct(rec.GetNormal(), scatterDir);

	//if (Vector3D::DotProduct(normal, scatterDir) < Float::NearZero) {
	//	absorb = true;
	//}
	//else {
	//	absorb = false;
	//}
	//emission = false;
	//transparent = false;

	//Vector3D unitDir = rayIn.GetDir();
	normal = rec.GetNormal();

	attentuation = m_albedo;

	absorb = true;
	emission = false;
	transparent = false;
}