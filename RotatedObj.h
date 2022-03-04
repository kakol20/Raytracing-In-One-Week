#pragma once
#include "Object.h"
class RotatedObj :
	public Object {
public:
	RotatedObj() : RotatedObj(Vector3D(), nullptr) {};
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="degrees">Angles in degrees</param>
	/// <param name="object"></param>
	RotatedObj(const Vector3D degrees, Object* object);

	virtual ~RotatedObj();

	virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

	virtual bool SphereIntersectGround(const Vector3D pos, const float radius);
	virtual bool SphereIntersectSphere(const Vector3D pos, const float radius);

private:
	Object* m_object;
	Vector3D m_radians;
};

