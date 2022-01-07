#pragma once
#include "Ray.h"
#include "Vector3D.h"

class Object {
public:
	Object();

	virtual bool Hit(Ray& Hit) = 0;

	const Vector3D GetPos();

	~Object();

private:

protected:
	Vector3D m_pos;

};