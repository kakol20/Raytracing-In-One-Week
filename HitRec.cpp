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

	m_bitangent = copyHitRec.m_bitangent;
	m_frontFace = copyHitRec.m_frontFace;
	m_mat = copyHitRec.m_mat;
	m_normal = copyHitRec.m_normal;
	m_point = copyHitRec.m_point;
	m_t = copyHitRec.m_t;
	m_tangent = copyHitRec.m_tangent;
	m_uv = copyHitRec.m_uv;

	/*bool m_frontFace;
	float m_t;
	Material* m_mat;
	Vector3D m_bitangent;
	Vector3D m_normal;
	Vector3D m_point;
	Vector3D m_tangent;
	Vector3D m_uv;*/

	return *this;
}

void HitRec::SetFaceNormal(Ray& ray, Vector3D& outwardNormal) {
	m_frontFace = Vector3D::DotProduct(ray.GetDir(), outwardNormal) < 0.f;
	m_normal = m_frontFace ? outwardNormal : outwardNormal * -1.f;
}

const Vector3D HitRec::TangentToWorld(const Vector3D tangentSpace) {
	Vector3D ts_normal = tangentSpace;
	Vector3D worldSpace = (m_tangent * ts_normal.GetX()) + (m_bitangent * ts_normal.GetY()) + (m_normal * ts_normal.GetZ());
	worldSpace.Normalize();
	return worldSpace;
	
}
