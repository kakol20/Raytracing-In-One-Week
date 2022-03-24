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
	~Raytracing();

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
	float m_clipEnd, m_clipStart;
	float m_hdriStrength;
	float m_nearZero;
	float m_noiseThreshold;
	int m_imageWidth, m_imageHeight, m_maxSamples, m_rayDepth, m_tileSize/*, m_shadowDepth*/, m_minSamples;
	int m_tilesRendered;
	size_t m_nextAvailable;
	size_t m_useThreads;
	std::fstream m_log;
	String m_fileFolder;
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
		unsigned int seed;
	};
	int m_xTileCount, m_yTileCount;

	void ShuffleTiles();

private:
	// ----- SCENE CREATION -----

	void CornellBox();
	void DebugScene();
	void FinalScene();
	void OriginalScene();
	void TexturedScene();

	// ----- RENDERING -----

	void RenderTile(const size_t startIndex);
	void Render(const int minX, const int minY, const int maxX, const int maxY);
	void ShowProgress();

	// raytracing
	Vector3D RayColor(Ray& ray, const int depth, bool & isBackground);
	const bool RayHitObject(Ray& ray, const float t_min, const float t_max, HitRec& rec);

	Vector3D EmissionColor(HitRec& rec);
	Vector3D ObjectColor(Ray& ray, HitRec& rec, Ray& scattered, bool& continueRay, bool& alpha);

	std::map<String, Image*> m_textures;
	std::map<String, Material*> m_matMap;
	std::map<String, Object*> m_unrenderedObjects;
	std::vector<Material*> m_matVec;
	std::vector<Object*> m_renderedObjects;
	std::vector<Tile> m_tiles;

	// ----- THREADING -----
	std::vector<std::thread> m_threads;
	std::map<std::thread::id, size_t> m_threadID;
};
