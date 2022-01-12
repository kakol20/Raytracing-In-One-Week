#pragma once
#include <vector>

#include "Ray.h"
#include "Vector3D.h"
#include "Object.h"
#include "Material.h"

struct HitRec {
	bool frontFace;
	float t;
	Material* mat_ptr;
	Vector3D normal;
	Vector3D point;

	inline HitRec() {
		point = Vector3D();
		normal = Vector3D();
		t = 0;
		frontFace = false;
	}

	inline void SetFaceNormal(Ray& ray, Vector3D& outwardNormal) {
		frontFace = ray.GetDirection().DotProduct(outwardNormal) < 0;
		normal = frontFace ? outwardNormal : outwardNormal * -1;

		normal = normal.UnitVector();
	}
};

class Object {
public:
	Object() {};

	virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec & rec) = 0;
	//virtual Vector3D RayColor(Ray& ray) = 0;

	const Vector3D GetPos() { return m_pos; };
	Material* GetMat() { return m_mat; };

	~Object() {};

private:

protected:
	Vector3D m_pos;
	Material* m_mat;

};