#ifndef SPHERE_H
#define SPHERE_H

#include "../../maths/Quaternion.h"

#include "Object.h"

class Sphere : public Object {
public:
	Sphere() : Sphere(1, nullptr, Vector3D::Zero, Vector3D::Zero) {};
	Sphere(const Float& radius, Material* mat, const Vector3D& rotation, const Vector3D& translation, bool flipNormals = false, const Vector3D& uvScale = Vector3D::One);
	virtual ~Sphere();

	virtual bool Hit(Ray& ray, const Float t_min, const Float t_max, HitRec& rec);

	virtual bool SphereIntersectGround(const Vector3D& pos, const Float radius) { return false; };
	virtual bool SphereIntersectSphere(const Vector3D& pos, const Float radius);

private:
	bool m_flipNormals;
	Float m_radius;
	Quaternion m_rotation;
	Quaternion m_rotationInv;

	bool TranslationHit(Ray& ray, const Float t_min, const Float t_max, HitRec& rec);
	bool RotationHit(Ray& ray, const Float t_min, const Float t_max, HitRec& rec);
	bool LocalHit(Ray& ray, const Float t_min, const Float t_max, HitRec& rec);

	Vector3D CalculateTangent(HitRec& rec);
	Vector3D CalculateUV(const Vector3D& point);
};

#endif // !SPHERE_H
