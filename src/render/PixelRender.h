#pragma once

#include <string>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "../utility/Color.h"

class PixelRender {
public:
	PixelRender(const unsigned int& width = 1280, const unsigned int& height = 720, const std::string& name = "Pixel Renderer");
	~PixelRender() {};

	bool Init();
	bool Draw();

	void SetPixel(const unsigned int& x, const unsigned int& y, const rt::Color& color);

	void UpdateTexture() {
		m_renderTexture.loadFromImage(m_renderImage);
		m_renderSprite.setTexture(m_renderTexture, true);
	};

private:
	sf::RenderWindow m_window;
	unsigned int m_width, m_height;

	sf::Image m_renderImage;
	sf::Texture m_renderTexture;
	sf::Sprite m_renderSprite;
};