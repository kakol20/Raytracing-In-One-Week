#pragma once
#include <vector>
#include <map>
#include <thread>
#include <fstream>

#include "Camera.h"
#include "Image.h"
#include "Light.h"
#include "Object.h"
#include "String.h"

class Raytracing {
public:
	Raytracing();

	void Init();

	void SetRenderMode(const char* renderMode);

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

	void RenderTile(const size_t startIndex);

	Vector3D BiLerp(const float x, const float y, Image& image);
	void UVSphere(Vector3D unitDir, float& u, float& v);

private: // member variables
	std::vector<Object*> m_objects;
	std::vector<std::thread> m_threads;
	std::map<String, Material*> m_materials;
	std::map<std::thread::id, size_t> m_threadId;
	std::vector<Material*> m_proceduralMats;
	std::vector<Tile> m_tiles;

	size_t m_nextAvailable;

	std::fstream m_log;

	Camera m_camera;
	Image m_render;
	Image m_hdri;

	//Light m_mainLight;

	std::vector<Light> m_lights;
	float m_hdriResolution;

	int m_imageWidth, m_imageHeight, m_samplesPerPixel, m_maxDepth, m_tileSize;
	int m_tilesRendered;
	float m_aperture, m_verticalFOV;

	bool m_renderNormals;
	bool m_renderAlbedo;

	String m_renderMode;
	bool m_debugMode;
};