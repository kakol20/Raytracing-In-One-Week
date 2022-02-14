#pragma once
#include <fstream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "Camera.h"
#include "HitRec.h"
#include "Image.h"
#include "Material.h"
#include "Object.h"
#include "Ray.h"
#include "String.h"
#include "String.h"
#include "Vector3D.h"

class Raytracing {
public:
	Raytracing();
	~Raytracing() {};

	bool Init();
	bool Run();
	bool RunMode();

private:
	Image m_hdri;
	Image m_render;

	Camera m_camera;

	//std::mutex m_mtx;
	bool m_shuffleTiles;
	float m_aperture, m_verticalFOV;
	float m_hdriStrength;
	float m_nearZero;
	int m_imageWidth, m_imageHeight, m_samplesPerPixel, m_rayDepth, m_tileSize, m_shadowDepth;
	int m_tilesRendered;
	size_t m_nextAvailable;
	size_t m_useThreads;
	std::fstream m_log;
	String m_renderMode;
	String m_renderScene;

private:
	struct Tile {
		int minX, minY, maxX, maxY;
		bool tileComplete;
		bool activeTile;
		Vector3D leftXTileColor;
		Vector3D rightXTileColor;
		int tileX, tileY;
	};
	int m_xTileCount, m_yTileCount;

	void ShuffleTiles();

private:
	// ----- SCENE CREATION -----

	void DebugScene();
	void FinalScene();
	void TexturedScene();

	// ----- RENDERING -----

	void RenderTile(const size_t startIndex);
	void Render(const int minX, const int minY, const int maxX, const int maxY);
	void ShowProgress();

	// raytracing
	Vector3D RayColor(Ray& ray, const int depth);
	const bool RayHitObject(Ray& ray, const float t_min, const float t_max, HitRec& rec);

	Vector3D EmissionColor(HitRec& rec);
	Vector3D ObjectColor(Ray& ray, HitRec& rec, Ray& scattered, bool& continueRay, bool& alpha);

	std::map<String, Material*> m_matMap;
	std::vector<Material*> m_matVec;
	std::vector<Object*> m_objects;
	std::vector<Tile> m_tiles;

	// ----- THREADING -----
	std::vector<std::thread> m_threads;
	std::map<std::thread::id, size_t> m_threadID;
};
