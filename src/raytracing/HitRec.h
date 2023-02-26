#ifndef HITREC_H
#define HITREC_H

#include "../maths/Vector3D.h"
#include "Ray.h"

class Material;

class HitRec {
public:
	HitRec();
	HitRec(const HitRec& copyHitRec);
	~HitRec();

	HitRec& operator=(const HitRec& copyHitRec);

	void SetFaceNormal(Ray& ray, Vector3D& outwardNormal);

	const bool GetFrontFace() const { return m_frontFace; };
	void SetFrontFace(const bool flag) { m_frontFace = flag; };

	const Float GetT() const { return m_t; };
	void SetT(const Float& t) { m_t = t; };

	Material* GetMat() { return m_mat; };
	void SetMat(Material* mat) { m_mat = mat; };

	const Vector3D GetNormal() const { return m_normal; };
	void SetNormal(const Vector3D& normal) { m_normal = normal; m_normal.Normalize(); };

	const Vector3D GetPoint() const { return m_point; };
	void SetPoint(const Vector3D& point) { m_point = point; };

	const Vector3D GetUV() const { return m_uv; };
	void SetUV(const Vector3D& uv) { m_uv = uv; };

	//const Vector3D GetTangent() const { return m_tangent; };
	const Vector3D GetTangent() const { return m_tangent; };
	void SetTangent(const Vector3D& tangent) { m_tangent = tangent; m_tangent.Normalize(); };

	const Vector3D GetBitangent() const { return m_bitangent; };
	void SetBitangent(const Vector3D& bitangent) { m_bitangent = bitangent; m_bitangent.Normalize(); };

	const Vector3D TangentToWorld(const Vector3D& tangentSpace) const;

private:
	bool m_frontFace;
	Float m_t;
	Material* m_mat;
	Vector3D m_bitangent;
	Vector3D m_normal;
	Vector3D m_point;
	Vector3D m_tangent;
	Vector3D m_uv;
};

#endif // !HITREC_H
