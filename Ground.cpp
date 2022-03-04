#include "Ground.h"

Ground::Ground() : Ground(0.f, nullptr) {
}

Ground::Ground(const float height, Material* mat, const Vector3D uvScale) {
	m_pos = Vector3D(0.f, height, 0.f);
	m_mat = mat;
	m_uvScale = uvScale;
}

Ground::~Ground() {
	m_mat = nullptr;
}

bool Ground::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	Vector3D p0 = m_pos;
	Vector3D n(0.f, 1.f, 0.f);

	Vector3D l0 = ray.GetOrig();
	Vector3D l = ray.GetDir();

	float lDotN = Vector3D::DotProduct(l, n);

	if (lDotN == 0.f) return false;

	// distance
	Vector3D p0SubL0 = p0 - l0;
	float d = Vector3D::DotProduct(p0SubL0, n) / lDotN;

	if (t_min > d || d > t_max) return false;

	// calculate point
	Vector3D p = ray.At(d);

	Vector3D axisClose = m_pos - p;
	axisClose *= Vector3D(0.f, 1.f, 0.f);

	if (!axisClose.NearZero()) return false;

	rec.SetT(d);
	rec.SetPoint(p);
	rec.SetMat(m_mat);
	rec.SetFaceNormal(ray, n);
	rec.SetUV(Vector3D(p.GetX(), p.GetZ(), 0.f) * m_uvScale);
	rec.SetTangents(Vector3D(1.f, 0.f, 0.f));

	return true;
}

bool Ground::SphereIntersectGround(const Vector3D pos, const float radius) {
	Vector3D spherePos = pos;
	float sphereHeight = spherePos.GetY() - m_pos.GetY();

	if (sphereHeight < radius) return true;
	return false;
}

bool Ground::SphereIntersectSphere(const Vector3D pos, const float radius) {
	return false;
}
