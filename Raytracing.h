#pragma once
#include "Image.h"
#include "Ray.h"
#include "Vector3D.h"

class Raytracing {
public:
	Raytracing();

	bool Run();

	~Raytracing();

private:
	const Vector3D RayColor(Ray& ray);

private: // member variables
};