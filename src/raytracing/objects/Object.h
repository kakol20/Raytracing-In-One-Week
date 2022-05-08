#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "../HitRec.h"
#include "../materials/Material.h"
#include "../Ray.h"

class Object {
public:
	Object() {};
	virtual ~Object() {
		m_mat = nullptr;
	}

	virtual bool Hit(Ray& ray, const Float& t_min, const Float& t_max, HitRec& rec) = 0;

	virtual bool SphereIntersectGround(const Vector3D& pos, const Float& radius) { return false; };
	virtual bool SphereIntersectSphere(const Vector3D& pos, const Float& radius) { return false; };

	const Vector3D GetPos() const { return m_pos; };

protected:
	Material* m_mat = nullptr;
	Vector3D m_pos;
	Vector3D m_uvScale;
};

#endif // !OBJECT_HPP