#pragma once
#include <vector>
#include <thread>

#include "Image.h"
#include "Ray.h"
#include "Vector3D.h"
#include "Object.h"
#include "Camera.h"

class Raytracing {
public:
	Raytracing();

	bool Run();

	~Raytracing();

private:
	const bool HitObject(Ray& ray, const float t_min, const float t_max, HitRec& rec);

	void Render(const int minX, const int minY, const int maxX, const int maxY);

	Vector3D RayColor(Ray& ray, const int depth);

private: // member variables
	std::vector<Object*> m_objects;
	std::vector<std::thread> m_threads;

	Camera m_camera;
	Image m_render;

	int m_imageWidth, m_imageHeight, m_samplesPerPixel, m_maxDepth, m_tileSize;
};