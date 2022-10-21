#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "../utility/BlueNoise.h"
#include "../utility/Color.h"
#include "raytracing/Camera.h"
#include "raytracing/HitRec.h"
#include "Settings.h"

class PixelRender {
private:
	enum class ColorSpace { Non_Color, sRGB };

public:
	PixelRender(const unsigned int& width = 1280, const unsigned int& height = 720, const std::string& name = "Pixel Renderer");
	~PixelRender() {};

	bool Init();
	void Update();
	bool Draw();

	void SetPixel(const unsigned int& x, const unsigned int& y, const rt::Color& color);

	void UpdateTexture();

	void SaveRender(const char* fileLocation);

private:
	sf::RenderWindow m_window;
	unsigned int m_width, m_height;

	sf::Image m_renderImage;
	sf::Texture m_renderTexture;
	sf::Sprite m_renderSprite;
	PixelRender::ColorSpace m_renderColSpce;

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
	std::map<std::thread::id, size_t> m_threadID;
	std::mutex m_mutex;
	std::vector<std::thread> m_threads;

	// ----- MAIN RENDERING FUNCTIONS -----

	bool RunMode();
	void RenderTile(const size_t& startIndex);
	void Render(const int& minX, const int& minY, const int& maxX, const int& maxY);

	bool RayHitObject(Ray& ray, const float& t_min, const float& t_max, HitRec& rec);
	sf::Vector3f RayColor(Ray& ray, const int& depth, const sf::Vector3f& initialRayCol = rt::Vector3::One);

	void ShowProgress();
};