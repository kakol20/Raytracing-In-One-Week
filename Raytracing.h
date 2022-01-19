#pragma once
#include <vector>
#include <map>
#include <thread>
#include <fstream>

#include "Camera.h"
#include "Image.h"
#include "Object.h"
#include "String.h"

class Raytracing {
public:
	Raytracing();

	void Init();

	bool Run();

	~Raytracing();

private:
	const Vector3D RayColor(Ray& ray, const int depth);

	const bool HitObject(Ray& ray, const float t_min, const float t_max, HitRec& rec);

	void Render(const int minX, const int minY, const int maxX, const int maxY);

private:
	struct Tile {
		int minX, minY, maxX, maxY;
	};

	std::vector<Tile> m_tiles;
	size_t m_nextAvailable;

	void RenderTile(const size_t startIndex);

	std::fstream m_log;

private: // member variables
	std::vector<Object*> m_objects;
	std::vector<std::thread> m_threads;
	std::map<String, Material*> m_materials;
	std::vector<Material*> m_proceduralMats;

	Camera m_camera;
	Image m_render;

	int m_imageWidth, m_imageHeight, m_samplesPerPixel, m_maxDepth, m_tileSize;
	float m_aperture, m_verticalFOV;
};