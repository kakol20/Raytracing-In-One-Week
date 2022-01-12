#include "HitRec.h"
HitRec::HitRec() {
	m_frontFace = false;
	m_mat = nullptr;
	m_normal = Vector3D();
	m_point = Vector3D();
	m_t = 0;
}

HitRec::HitRec(const HitRec& copyHitRec) {
	m_frontFace = copyHitRec.m_frontFace;
	m_normal = copyHitRec.m_normal;
	m_point = copyHitRec.m_point;
	m_t = copyHitRec.m_t;
}

HitRec::~HitRec() {
	m_mat = nullptr;
}

HitRec& HitRec::operator=(const HitRec& copyHitRec) {
	if (&copyHitRec == this) return *this;

	m_frontFace = copyHitRec.m_frontFace;
	m_normal = copyHitRec.m_normal;
	m_point = copyHitRec.m_point;
	m_t = copyHitRec.m_t;

	return *this;
}

void HitRec::SetFaceNormal(Ray& ray, Vector3D& outwardNormal) {
	m_frontFace = ray.GetDirection().DotProduct(outwardNormal) < 0.0f;
	m_normal = m_frontFace ? outwardNormal : outwardNormal * -1.0f;

	m_normal = m_normal.UnitVector();
}

const bool HitRec::GetFrontFace() {
	return m_frontFace;
}

void HitRec::SetFrontFace(const bool frontFace) {
	m_frontFace = frontFace;
}

Material* HitRec::GetMaterial() {
	return m_mat;
}

void HitRec::SetMaterial(Material* mat) {
	m_mat = mat;
}

const Vector3D HitRec::GetNormal() {
	return m_normal;
}

void HitRec::SetNormal(const Vector3D normal) {
	m_normal = normal;
}

const Vector3D HitRec::GetPoint() {
	return m_point;
}

void HitRec::SetPoint(const Vector3D point) {
	m_point = point;
}

const float HitRec::GetT() {
	return m_t;
}

void HitRec::SetT(const float t) {
	m_t = t;
}
