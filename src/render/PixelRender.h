#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "../utility/BlueNoise.h"
#include "../utility/Colour.h"
#include "../utility/ImageWrapper.h"
#include "raytracing/Camera.h"
#include "raytracing/HitRec.h"
#include "Settings.h"

class PixelRender {

public:
	PixelRender(const unsigned int& width = 1280, const unsigned int& height = 720, const std::string& name = "Pixel Renderer");
	~PixelRender() {};

	bool Init();
	void Update();
	bool Draw();

	void SetPixel(const unsigned int& x, const unsigned int& y, const rt::Colour& color);

	void UpdateTexture();

	void SaveRender(const char* fileLocation);

private:
	sf::RenderWindow m_window;
	unsigned int m_width, m_height;

	sf::Image m_renderImage;
	sf::Texture m_renderTexture;
	sf::Sprite m_renderSprite;
	rt::Image::ColorSpace m_renderColSpce;
	bool m_renderScene;

	Camera m_camera;
	float m_clipEnd;
	float m_clipStart;

	Settings m_settings;
	BlueNoise m_blueNoise;

	std::fstream m_log;
	std::string m_fileFolder;
	int m_tilesRendered;
	size_t m_nextAvailable;

	// ----- TILES -----

	struct Tile {
		int minX, minY, maxX, maxY;
		bool tileComplete;
		bool activeTile;
		int tileX, tileY;
		unsigned int seed;
	};

	int m_xTileCount, m_yTileCount;
	std::vector<Tile> m_tiles;

	// ----- MULTI_THREADING -----

	size_t m_useThreads;
	//std::map<sf::Thread::id, size_t> m_threadID;
	sf::Mutex m_mutex;
	std::vector<sf::Thread> m_threads;
	std::vector<size_t> m_threadArg;

	// ----- MAIN RENDERING FUNCTIONS -----

	bool RunMode();
	void RenderTile();
	void Render(const int& minX, const int& minY, const int& maxX, const int& maxY);

	bool RayHitObject(Ray& ray, const float& t_min, const float& t_max, HitRec& rec);
	rt::Colour RayColor(Ray& ray, const int& depth);
};