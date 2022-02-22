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
	m_normal = m_frontFace ? outwardNormal : -outwardNormal;
}

const Vector3D HitRec::TangentToWorld(const Vector3D tangentSpace) {
	Matrix3x3 matrix(m_tangent, m_bitangent, m_normal);
	matrix.Normalize();

	Vector3D worldSpaceNormal = matrix * tangentSpace;
	worldSpaceNormal.Normalize();
	return worldSpaceNormal;
}

void HitRec::SetTangents(const Vector3D tangent) {
	m_tangent = tangent;
	m_tangent.Normalize();
	m_normal.Normalize();
	m_bitangent = Vector3D::CrossProduct(m_normal, m_tangent);
	//m_bitangent = Vector3D::CrossProduct(m_tangent, m_normal);
}
