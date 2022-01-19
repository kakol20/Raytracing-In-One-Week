#pragma once
#include "Vector3D.h"
#include "Ray.h"

class Material;

class HitRec {
public:
	HitRec();
	HitRec(const HitRec& copyHitRec);
	~HitRec();

	HitRec& operator=(const HitRec& copyHitRec);

	void SetFaceNormal(Ray& ray, Vector3D& outwardNormal);

public:

	const bool GetFrontFace();
	void SetFrontFace(const bool frontFace);

	Material* GetMaterial();
	void SetMaterial(Material* mat);

	Vector3D GetNormal();
	void SetNormal(const Vector3D normal);

	Vector3D GetPoint();
	void SetPoint(const Vector3D point);

	const float GetT();
	void SetT(const float t);

private:
	bool m_frontFace;
	float m_t;
	Material* m_mat;
	Vector3D m_normal;
	Vector3D m_point;
};