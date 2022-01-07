#pragma once
#include <vector>

#include "Image.h"
#include "Ray.h"
#include "Vector3D.h"
#include "Object.h"

class Raytracing {
public:
	Raytracing();

	bool Run();

	~Raytracing();

private:
	const Vector3D RayColor(Ray& ray);

	std::vector<Object*> m_objects;

private:
	const bool HitObject(Ray& ray, const float t_min, const float t_max, HitRec& rec);

private: // member variables
};