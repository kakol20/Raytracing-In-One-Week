#include <algorithm>

#include "../utility/Color.h"
#include "../utility/Random.h"

#include "PixelRender.h"

PixelRender::PixelRender(const unsigned int& width, const unsigned int& height, const std::string& name) :
	m_width(width),
	m_height(height),
	m_window(sf::VideoMode(width, height), name, sf::Style::Titlebar | sf::Style::Close) {
	

	m_clipEnd = 1000.f;
	m_clipStart = 1e-6f;
}

bool PixelRender::Init() {
	// ----- SETTINGS -----

	if (!m_settings.Read("settings.cfg")) m_settings.Write("settings.cfg");

	Random::Seed = (unsigned int)std::stoul(m_settings["randomSeed"]);
	if (Random::Seed == 0) Random::Seed = 0xACE1u;

	if (std::stof(m_settings["verticalFOV"]) > 180.f) m_settings["verticalFOV"] = "179";
	if (std::stof(m_settings["verticalFOV"]) <= 0.f) m_settings["verticalFOV"] = "1";

	m_width = std::stoi(m_settings["imageWidth"]);
	m_height = std::stoi(m_settings["imageHeight"]);

	m_window.setSize({ m_width, m_height });
	sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(m_width), static_cast<float>(m_height));
	m_window.setView(sf::View(visibleArea));

	// ----- INITIALISE SCENE -----

	if (m_settings["scene"] == "none") {
		// -- EMPTY FOR NOW --
	}
	else {
		// -- DEBUG SCENE --
		sf::Vector3f lookFrom = { 0.f, 2.f, 5.f };

		const float aspectRatio = static_cast<float>(m_width) / m_height;

		m_settings["blurStrength"] = "0";
		m_camera = Camera(aspectRatio, std::stof(m_settings["blurStrength"]), 10.f, std::stof(m_settings["verticalFOV"]), lookFrom, rt::Vector3::Zero, rt::Vector3::Up);

		m_clipEnd = 1000.f;

		m_renderImage.create(m_width, m_height, sf::Color::Black);
	}

	// -- temporary --
	for (unsigned int x = 0; x < m_width; x++) {
		for (unsigned int y = 0; y < m_height; y++) {
			Ray rayDir = m_camera.GetRay(x / (m_width - 1.f), y / (m_height - 1.f));
			sf::Vector3f dir = rayDir.GetDir() + sf::Vector3f(1.f, 1.f, 1.f);
			dir /= 2.f;
			dir *= 255.f;

			rt::Color color(dir.x, dir.y, dir.z);

			SetPixel(x, y, color);
		}
	}

	// ----- GENERATE BLUE NOISE -----

	int minSamples = std::stoi(m_settings["minSamples"]);
	int maxSamples = std::stoi(m_settings["maxSamples"]);

	if (minSamples < 256) {
		int sampleCount = maxSamples < 256 ? maxSamples : 256;

		maxSamples = sampleCount;
		m_blueNoise.Generate(sampleCount, 10);
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
	//if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
	//	sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
	//	/*mousePos.x = std::clamp(mousePos.x, 0, static_cast<int>(m_width) - 1);
	//	mousePos.y = std::clamp(mousePos.y, 0, static_cast<int>(m_height) - 1);*/

	//	for (int x = -1; x <= 1; x++) {
	//		for (int y = -1; y <= 1; y++) {
	//			int l_x = std::clamp(mousePos.x + x, 0, static_cast<int>(m_width) - 1);
	//			int l_y = std::clamp(mousePos.y + y, 0, static_cast<int>(m_height) - 1);

	//			SetPixel(l_x, l_y, rt::Color((sf::Uint8)255, 255, 255));
	//		}
	//	}

	//	UpdateTexture();
	//}
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