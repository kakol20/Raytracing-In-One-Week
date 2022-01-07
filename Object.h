#pragma once
#include "Ray.h"
#include "Vector3D.h"

class Object {
public:
	Object() {};

	virtual float Hit(Ray& ray) = 0;
	virtual Vector3D RayColor(Ray& ray) = 0;

	const Vector3D GetPos() { return m_pos; };

	~Object() {};

private:

protected:
	Vector3D m_pos;

};