#include "PointLight.h"

#include "../../misc/Random.h"

PointLight::PointLight(const Unshaded& mat, const Float radius, const Vector3D& pos) {
	m_mat = nullptr;
	m_radius = radius;
	m_pos = pos;
	m_light_mat = mat;
}

bool PointLight::Hit(Ray& ray, const Float t_min, const Float t_max, HitRec& rec) {
	Vector3D oc = ray.GetOrig() - m_pos;
	Vector3D dir = ray.GetDir();
	dir.Normalize();

	// shortest vector must be as Dot(dir, sv) = 0
	// closest point = oc + (l * dir)

	Float l = Vector3D::DotProduct(oc, dir) / dir.Magnitude();

	Vector3D point = oc + (dir * l);
	point += m_pos;

	oc += m_pos;

	// check if point is not behind the ray origin
	if (Vector3D::DotProduct(dir, point - oc) > 0) {
		// check if point lies within t_min and t_max
		if (l >= t_min && l <= t_max) {
			Float rand = Random::RandomFloat(0, m_radius);
			if (l < rand) {
				rec.SetT(l);
				rec.SetMat(&m_light_mat);
				rec.SetPoint(point);

				return true;
			}
		}
	}
	return false;
}
