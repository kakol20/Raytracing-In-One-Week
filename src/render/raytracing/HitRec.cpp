#include "HitRec.h"

HitRec::HitRec() {
	m_frontFace = false;
	m_t = 0.f;
	m_mat = nullptr;
}

HitRec::HitRec(const HitRec& copyHitRec) {
	m_bitangent = copyHitRec.m_bitangent;
	m_frontFace = copyHitRec.m_frontFace;
	m_mat = copyHitRec.m_mat;
	m_normal = copyHitRec.m_normal;
	m_point = copyHitRec.m_point;
	m_t = copyHitRec.m_t;
	m_tangent = copyHitRec.m_tangent;
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

	return *this;
}

void HitRec::SetFaceNormal(Ray& ray, sf::Vector3f& outwardNormal) {
	m_frontFace = rt::Vector3::DotProduct(ray.GetDir(), outwardNormal);
	m_normal = m_frontFace ? outwardNormal : outwardNormal * -1.f;
}

const sf::Vector3f HitRec::TangentToWorld(const sf::Vector3f& tangentSpace) const {
	sf::Vector3f worldSpace = (m_tangent * tangentSpace.x) + (m_bitangent * tangentSpace.y) + (m_normal * tangentSpace.z);
	return rt::Vector3::Normalize(worldSpace);
}
