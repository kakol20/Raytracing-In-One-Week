#include "HitRec.h"

HitRec::HitRec() {
	m_frontFace = false;
	m_t = 0.f;
	m_mat = nullptr;
}

HitRec::HitRec(const HitRec& copyHitRec) {
	m_frontFace = copyHitRec.m_frontFace;
	m_t = copyHitRec.m_t;
	m_mat = copyHitRec.m_mat;
	m_normal = copyHitRec.m_normal;
	m_point = copyHitRec.m_point;
	m_uv = copyHitRec.m_uv;
}

HitRec::~HitRec() {
	m_mat = nullptr;
}

HitRec& HitRec::operator=(const HitRec& copyHitRec) {
	if (&copyHitRec == this) return *this;

	m_frontFace = copyHitRec.m_frontFace;
	m_t = copyHitRec.m_t;
	m_mat = copyHitRec.m_mat;
	m_normal = copyHitRec.m_normal;
	m_point = copyHitRec.m_point;
	m_uv = copyHitRec.m_uv;

	return *this;
}

void HitRec::SetFaceNormal(Ray& ray, Vector3D& outwardNormal) {
	m_frontFace = Vector3D::DotProduct(ray.GetDir(), outwardNormal) < 0.f;
	m_normal = m_frontFace ? outwardNormal : -outwardNormal;
}
