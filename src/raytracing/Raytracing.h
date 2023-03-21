#ifndef RAYTRACING_H
#define RAYTRACING_H

//#define HDR_BACKGROUND

#include <fstream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "../misc/BlueNoise.h"
#include "../wrapper/HDR.h"
#include "../wrapper/Image.h"
#include "Camera.h"
#include "materials/Material.h"
#include "objects/Object.h"
#include "Settings.h"

class Raytracing {
public:
	Raytracing();
	~Raytracing();

	bool Init();
	bool Run();

private:
	// ----- MEMBER VARIABLES -----

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
	std::vector<Tile> m_tiles;

	Settings m_settings;

	Camera m_camera;

	Image m_render;

#ifdef HDR_BACKGROUND
	HDR m_background;
#else
	Image m_background;
#endif // HDR_BACKGROUND

	BlueNoise m_blueNoise;
	Float m_bgStrength;
	Float m_clipEnd;
	Float m_clipStart;
	int m_tilesRendered;
	size_t m_nextAvailable;
	std::fstream m_log;
	std::string m_fileFolder;

	// multi threading

	size_t m_useThreads;
	std::map<std::thread::id, size_t> m_threadID;
	std::mutex m_mutex;
	std::vector<std::thread> m_threads;
	std::chrono::steady_clock::time_point m_lastUpdate;

	// ----- OBJECTS -----

	std::map<std::string, Material*> m_matMap;

	std::vector<Object*> m_renderedObjects;

private:
	// ----- MAIN RENDERING FUNCTIONS -----

	bool RunMode();
	void RenderTile(const size_t startIndex);
	void Render(const int minX, const int minY, const int maxX, const int maxY);

	bool RayHitObject(Ray& ray, const Float t_min, const Float t_max, HitRec& rec);
	Vector3D RayColor(Ray& ray, const int depth, const Vector3D& initialRayCol = Vector3D::One);

	// ----- INITIALISING SCENES -----

	void OriginalScene();
	void DebugScene();

	// ----- OTHER -----

	void ShowProgress();
	void ShuffleTiles();
};
#endif // !RAYTRACING_H
