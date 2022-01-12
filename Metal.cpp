#include "Metal.h"

Metal::Metal(const Vector3D& color) {
	m_albedo = color;
}

bool Metal::Scatter(Ray& rayIn, HitRec& rec, Vector3D& attenuation, Ray& scattered) {
	Vector3D reflected = Reflect(rayIn.GetDirection().UnitVector(), rec.normal);
	
	scattered = Ray(rec.point, reflected);
	attenuation = m_albedo;
	return scattered.GetDirection().DotProduct(rec.normal) > 0;
}

Vector3D Metal::Reflect(const Vector3D& v, const Vector3D& n) {
	return Vector3D();
}
