#include "Glass.h"
#include "LinearFeedbackShift.h"
#include <cmath>
#include <mutex>

Glass::Glass() {
	m_albedo = Vector3D(1.0f, 1.0f, 1.0f);
	m_ior = 1.45f;
	m_roughness = 0.0f;
}

Glass::Glass(const Vector3D albedo, const float ior, const float roughness) {
	m_albedo = albedo;
	m_ior = ior;
	m_roughness = roughness;
}

bool Glass::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered) {
	attentuation = m_albedo;

	float refractionRatio = rec.GetFrontFace() ? (1.0f / m_ior) : m_ior;

	Vector3D unitDir = rayIn.GetDirection().UnitVector();
	Vector3D unitDirInv = unitDir * -1.0f;
	float cosTheta = fmin(unitDirInv.DotProduct(rec.GetNormal()), 1.0f);
	float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);

	bool cannotRefract = refractionRatio * sinTheta > 1.0f;

	cannotRefract = cannotRefract || Reflectance(cosTheta, refractionRatio) > LinearFeedbackShift::RandFloat(32);

	Vector3D dir;

	if (cannotRefract) {
		dir = Reflected(unitDir, rec.GetNormal());
	}
	else {
		dir = Refract(unitDir, rec.GetNormal(), refractionRatio);
	}

	scattered = Ray(rec.GetPoint(), dir + (Vector3D::RandomInUnitSphere(16) * m_roughness));
	return true;
}

const float Glass::Reflectance(float cosine, float refIndex) {
	// Use Schlick's approximation for reflectance.
	float r0 = (1.0f - refIndex) / (1.0f + refIndex);
	r0 *= r0;
	return r0 + (1.0f - r0) * powf(1.0f - cosine, 5.0f);
}

Vector3D Glass::Refract(const Vector3D uv, const Vector3D n, const float eoe) {
	Vector3D uvInv = uv * -1.0f;
	float cosTheta = fmin(uvInv.DotProduct(n), 1.0f);

	Vector3D rOutPerp = (uv + (n * cosTheta)) * eoe;
	Vector3D rOutPara = n * -sqrtf(fabs(1.0f - rOutPerp.SqrMagnitude()));
	return rOutPerp + rOutPara;
}
