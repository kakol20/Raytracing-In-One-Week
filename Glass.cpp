#include "Glass.h"
#include "LinearFeedbackShift.h"
#include <mutex>

Glass::Glass() {
	m_albedo = Vector3D(1.0f, 1.0f, 1.0f);
	m_ior = 1.45f;
	m_roughness = 0.0f;
}

Glass::Glass(const Vector3D albedo, const float roughness, const float ior) {
	m_albedo = albedo;
	m_ior = ior;
	m_roughness = roughness;
}

bool Glass::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	// http://viclw17.github.io/2018/08/05/raytracing-dielectric-materials/

	attentuation = m_albedo;

	Vector3D outwardNormal;
	Vector3D reflected = Reflected(rayIn.GetDirection(), rec.GetNormal());
	Vector3D refracted;

	float n1OverN2, reflectProb, cosine;

	// Dealing with Ray Enter/Exit Object
	if (rayIn.GetDirection().DotProduct(rec.GetNormal()) > 0.0f) {
		// ray shoot through object into vacuum
		outwardNormal = rec.GetNormal() * -1.0f;
		n1OverN2 = m_ior;
		cosine = rayIn.GetDirection().UnitVector().DotProduct(rec.GetNormal());
	}
	else {
		// ray shoots into object
		outwardNormal = rec.GetNormal();
		n1OverN2 = 1.0f / m_ior;
		cosine = rayIn.GetDirection().UnitVector().DotProduct(rec.GetNormal()) * -1.0f;
	}

	// Dealing with Ray Reflection/Refraction (Fresnel)
	if (Refract(rayIn.GetDirection(), outwardNormal, n1OverN2, refracted)) {
		reflectProb = Schlick(cosine, m_ior);
	}
	else {
		reflectProb = 1.0f;
	}

	if (LinearFeedbackShift::RandFloat(32) < reflectProb) {
		scattered = Ray(rec.GetPoint(), reflected + (Vector3D::RandomInUnitSphere(32) * m_roughness));
	}
	else {
		scattered = Ray(rec.GetPoint(), refracted + (Vector3D::RandomInUnitSphere(32) * m_roughness));
	}

	return true;
}
