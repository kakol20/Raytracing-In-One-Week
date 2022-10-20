#include <algorithm>

#include "../utility/Color.h"

#include "PixelRender.h"

PixelRender::PixelRender(const unsigned int& width, const unsigned int& height, const std::string& name) :
	m_width(width),
	m_height(height),
	m_window(sf::VideoMode(width, height), name, sf::Style::Titlebar | sf::Style::Close) {
	// constructor
}

bool PixelRender::Init() {
	m_renderImage.create(m_width, m_height, sf::Color::Black);

	for (int x = 0; x < static_cast<int>(m_width) / 2; x++) {
		for (int y = 0; y < static_cast<int>(m_height); y++) {
			rt::Color col({ (x / static_cast<float>(m_width)) * 255.f, (y / static_cast<float>(m_height)) * 255.f , 0.f });

			SetPixel(x, y, col);
		}
	}

	for (int x = static_cast<int>(m_width) / 2; x < static_cast<int>(m_width); x++) {
		for (int y = 0; y < static_cast<int>(m_height); y++) {
			float gradient = (y / static_cast<float>(m_height)) * 64.f;
			rt::Color col({ gradient, gradient, gradient });

			SetPixel(x, y, col);
		}
	}

	UpdateTexture();

	return true;
}

void PixelRender::Update() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			m_window.close();
			return;
		}
	}

	// ----- MAIN LOOP -----

	// for testing purposes
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
		/*mousePos.x = std::clamp(mousePos.x, 0, static_cast<int>(m_width) - 1);
		mousePos.y = std::clamp(mousePos.y, 0, static_cast<int>(m_height) - 1);*/

		for (int x = -1; x <= 1; x++) {
			for (int y = -1; y <= 1; y++) {
				int l_x = std::clamp(mousePos.x + x, 0, static_cast<int>(m_width) - 1);
				int l_y = std::clamp(mousePos.y + y, 0, static_cast<int>(m_height) - 1);

				SetPixel(l_x, l_y, rt::Color((sf::Uint8)255, 255, 255));
			}
		}

		UpdateTexture();
	}
}

bool PixelRender::Draw() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			m_window.close();
			return false;
		}
	}

	//if (sf::Mouse::)

	// ----- DRAWING -----

	m_window.clear();

	m_window.draw(m_renderSprite);

	m_window.display();

	return m_window.isOpen();
}

void PixelRender::SetPixel(const unsigned int& x, const unsigned int& y, const rt::Color& color) {
	rt::Color col = color;
	col.Dither(x, y, 255);
	m_renderImage.setPixel(x, y, col.GetSFColor());
}

void PixelRender::SaveRender(const char* fileLocation) {
	m_renderImage.saveToFile(fileLocation);
}
