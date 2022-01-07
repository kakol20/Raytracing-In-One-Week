#pragma once
#include "Ray.h"
#include "Vector3D.h"
#include "Object.h"

struct HitRec {
	Vector3D point;
	Vector3D normal;
	float t;
	bool frontFace;

	inline HitRec() {
		point = Vector3D();
		normal = Vector3D();
		t = 0;
		frontFace = false;
	}

	inline void SetFaceNormal(Ray& ray, Vector3D& outwardNormal) {
		frontFace = ray.GetDirection().DotProduct(outwardNormal) < 0;
		normal = frontFace ? outwardNormal : outwardNormal * -1;
	}
};

class Object {
public:
	Object() {};

	virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec & rec) = 0;
	//virtual Vector3D RayColor(Ray& ray) = 0;

	const Vector3D GetPos() { return m_pos; };

	~Object() {};

private:

protected:
	Vector3D m_pos;

};