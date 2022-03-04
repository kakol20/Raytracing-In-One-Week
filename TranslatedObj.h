#pragma once
#include "Object.h"
class TranslatedObj :
	public Object {

public:
	TranslatedObj() : TranslatedObj(Vector3D(), nullptr) {};
	TranslatedObj(const Vector3D translation, Object* object);
	virtual ~TranslatedObj();

	virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

	virtual bool SphereIntersectGround(const Vector3D pos, const float radius);
	virtual bool SphereIntersectSphere(const Vector3D pos, const float radius);

private:
	Object* m_object;
};

