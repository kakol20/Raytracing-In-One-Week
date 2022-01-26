#include "Ground.h"

Ground::Ground(const float height, Material* mat) {
	m_pos = Vector3D(0.0f, height, 0.0f);
	m_mat = mat;
}

bool Ground::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	Vector3D p0 = m_pos;
	Vector3D n = Vector3D(0.0f, 1.0f, 0.0f);
	Vector3D l0 = ray.GetOrigin();
	Vector3D l = ray.GetDirection();

	float lDotN = l.DotProduct(n);

	if (lDotN == 0) return false;

	// calculate distance
	Vector3D p0SubL0 = p0 - l0;
	float d = p0SubL0.DotProduct(n) / lDotN;

	if (t_min > d || d > t_max) return false;

	// calculate point
	Vector3D p = l0 + (l * d);

	rec.SetT(d);
	rec.SetPoint(p);
	rec.SetMaterial(m_mat);
	rec.SetNormal(n);

	return true;
}
