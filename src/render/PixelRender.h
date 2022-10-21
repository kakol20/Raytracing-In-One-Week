#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "../utility/BlueNoise.h"
#include "../utility/Color.h"
#include "raytracing/Camera.h"
#include "Settings.h"

class PixelRender {
public:
	PixelRender(const unsigned int& width = 1280, const unsigned int& height = 720, const std::string& name = "Pixel Renderer");
	~PixelRender() {};

	bool Init();
	void Update();
	bool Draw();

	void SetPixel(const unsigned int& x, const unsigned int& y, const rt::Color& color);

	void UpdateTexture() {
		m_renderTexture.loadFromImage(m_renderImage);
		m_renderSprite.setTexture(m_renderTexture, true);
	};

	void SaveRender(const char* fileLocation);

private:
	sf::RenderWindow m_window;
	unsigned int m_width, m_height;

	sf::Image m_renderImage;
	sf::Texture m_renderTexture;
	sf::Sprite m_renderSprite;

	Camera m_camera;
	float m_clipEnd;
	float m_clipStart;

	Settings m_settings;
	BlueNoise m_blueNoise;

	struct Tile {
		int minX, minY, maxX, maxY;
		bool tileComplete;
		bool activeTile;
		int tileX, tileY;
		unsigned int seed;
	};

	int m_xTileCount, m_yTileCount;
	std::vector<Tile> m_tiles;
};