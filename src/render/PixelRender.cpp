#include <algorithm>
#include <chrono>

#include "../utility/Color.h"
#include "../utility/Random.h"

#include "PixelRender.h"

PixelRender::PixelRender(const unsigned int& width, const unsigned int& height, const std::string& name) :
	m_width(width),
	m_height(height),
	m_window(sf::VideoMode(width, height), name, sf::Style::Titlebar | sf::Style::Close) {

	m_clipEnd = 1000.f;
	m_clipStart = 1e-6f;

	m_useThreads = 0;
	m_xTileCount = 0;
	m_yTileCount = 0;
	m_nextAvailable = 0;
	m_tilesRendered = 0;

	m_renderColSpce = PixelRender::ColorSpace::Non_Color;
}

bool PixelRender::Init() {
	// ----- SETTINGS -----

	if (!m_settings.Read("settings.cfg")) m_settings.Write("settings.cfg");

	Random::Seed = (unsigned int)std::stoul(m_settings["randomSeed"]);
	if (Random::Seed == 0) Random::Seed = 0xACE1u;

	if (std::stof(m_settings["verticalFOV"]) > 180.f) m_settings["verticalFOV"] = "179";
	if (std::stof(m_settings["verticalFOV"]) <= 0.f) m_settings["verticalFOV"] = "1";

	m_width = std::stoi(m_settings["m_width"]);
	m_height = std::stoi(m_settings["m_height"]);

	int tileSize = std::stoi(m_settings["tileSize"]);

	m_window.setSize({ m_width, m_height });
	sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(m_width), static_cast<float>(m_height));
	m_window.setView(sf::View(visibleArea));

	// ----- INITIALISE SCENE -----

	if (m_settings["scene"] == "none") {
		// -- EMPTY FOR NOW --
	}
	else {
		// -- DEBUG SCENE --

		m_fileFolder = "render/debug/";

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

	// ----- GENERATE TILES -----

	int maxXTiles = static_cast<int>(m_width) < tileSize ? 1 : static_cast<int>(m_width) / tileSize;
	int maxYTiles = static_cast<int>(m_height) < tileSize ? 1 : static_cast<int>(m_height) / tileSize;
	int maxTiles = maxXTiles * maxYTiles;

	int xTileSize = static_cast<int>(std::roundf(static_cast<float>(m_width) / maxXTiles));
	int yTileSize = static_cast<int>(std::roundf(static_cast<float>(m_height) / maxYTiles));

	int widthModulo = m_width % maxXTiles;
	int heightModulo = m_height % maxYTiles;

	int y = 0;
	int countY = 0;
	while (y < static_cast<int>(m_height)) {
		m_yTileCount++;

		int addY = heightModulo > 0 ? yTileSize + 1 : yTileSize;
		int maxY = y + addY;
		maxY = maxY > static_cast<int>(m_height) ? static_cast<int>(m_height) : maxY;

		int l_widthModulo = widthModulo;

		int x = 0;
		int xTileCount = 0;
		int countX = 0;
		while (x < static_cast<int>(m_width)) {
			xTileCount++;

			int addX = l_widthModulo > 0 ? xTileSize + 1 : xTileSize;
			int maxX = x + addX;
			maxX = maxX > static_cast<int>(m_width) ? static_cast<int>(m_width) : maxX;

			m_tiles.push_back({ x, y, maxX, maxY, false, false, countX, countY, Random::RandomUInt() });

			x = maxX;
			l_widthModulo--;
			countX++;
		}

		m_xTileCount = xTileCount;

		y = maxY;
		heightModulo--;
		countY++;
	}

	std::reverse(m_tiles.begin(), m_tiles.end());

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

	auto start = std::chrono::high_resolution_clock::now();

	if (m_settings["renderMode"] == "color" || m_settings["renderMode"] == "emission" || m_settings["renderMode"] == "normal" || m_settings["renderMode"] == "albedo") {
		RunMode();
	}
	else if (m_settings["renderMode"] == "all") {
		m_settings["renderMode"] = "normal";
		RunMode();

		m_settings["renderMode"] = "emission";
		RunMode();

		m_settings["renderMode"] = "albedo";
		RunMode();

		m_settings["renderMode"] = "color";
		RunMode();
	}
	else {
		m_settings["renderMode"] = "color";
		RunMode();
	}

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsedSec = end - start;
	std::chrono::duration<float, std::ratio<60, 1>> elapsedMin = end - start;

	std::fstream runTime;
	std::string fileLocation = m_fileFolder;
	fileLocation += "runTime.txt";
	runTime.open(fileLocation.c_str(), std::ios_base::out);
	if (runTime.is_open()) {
		runTime << "Elapsed time in seconds: " << elapsedSec << "\n"
			<< "Elapsed time in minutes: " << elapsedMin << "\n";

		runTime.close();
	}
}

bool PixelRender::Draw() {
	m_mutex.lock();

	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			m_window.close();

			m_mutex.unlock();
			return false;
		}
	}

	//if (sf::Mouse::)

	// ----- DRAWING -----

	m_window.clear();

	m_window.draw(m_renderSprite);

	m_window.display();

	m_mutex.unlock();

	return m_window.isOpen();
}

void PixelRender::SetPixel(const unsigned int& x, const unsigned int& y, const rt::Color& color) {
	rt::Color col = color;
	col.Dither(x, y, 255);

	m_mutex.lock();
	m_renderImage.setPixel(x, y, col.GetSFColor());
	m_mutex.unlock();
}

void PixelRender::UpdateTexture() {
	m_mutex.lock();
	m_renderTexture.loadFromImage(m_renderImage);
	m_renderSprite.setTexture(m_renderTexture, true);
	m_mutex.unlock();
}

void PixelRender::SaveRender(const char* fileLocation) {
	m_renderImage.saveToFile(fileLocation);
}

bool PixelRender::RunMode() {
	if (m_settings["renderMode"] == "color" || m_settings["renderMode"] == "albedo" || m_settings["renderMode"] == "emission") {
		m_renderColSpce = PixelRender::ColorSpace::sRGB;
	}
	else {
		m_renderColSpce = PixelRender::ColorSpace::Non_Color;
	}

	for (auto it = m_tiles.begin(); it != m_tiles.end(); it++) {
		PixelRender::Tile& tile = (*it);

		tile.activeTile = false;
		tile.tileComplete = false;
	}

	m_tilesRendered = 0;
	m_nextAvailable = 0;

	// ----- MAIN RENDER -----

	// -- LOGGING --

	std::string output = m_fileFolder;

	if (m_settings["renderMode"] == "color") {
		output += "log_color.txt";
	}
	else if (m_settings["renderMode"] == "emission") {
		output += "log_emission.txt";
	}
	else if (m_settings["renderMode"] == "normal") {
		output += "log_normal.txt";
	}
	else if (m_settings["renderMode"] == "albedo") {
		output += "log_albedo.txt";
	}
	else {
		output += "log_color.txt";
	}

	m_log.open(output.c_str(), std::ios_base::out);
	m_log << "Threads Used: " << m_useThreads << "\nTotal tiles: " << m_tiles.size() << "\n";

	ShowProgress();

	// ------ MULTI THREADING -----

	if (m_useThreads > 1) {
		for (size_t i = 0; i < m_useThreads; i++) {
			m_threads.push_back(std::thread(&PixelRender::RenderTile, this, m_nextAvailable));
			m_threadID[m_threads[i].get_id()] = i;

			//m_threads[i].sleep
			m_nextAvailable++;
		}
	}
	else {
		RenderTile(0);
	}

	// -- CHECK FOR THREAD FINISH --

	if (m_useThreads > 1) {
		for (auto it = m_threads.begin(); it != m_threads.end(); it++) {
			(*it).join();
		}
	}

	// ----- SAVE RENDER -----

	output = m_fileFolder;
	if (m_settings["renderMode"] == "color") {
		output += "render_c.png";
	}
	else if (m_settings["renderMode"] == "emission") {
		output += "render_e.png";
	}
	else if (m_settings["renderMode"] == "normal") {
		output += "render_n.png";
	}
	else if (m_settings["renderMode"] == "albedo") {
		output += "render_a.png";
	}
	else {
		output += "render_c.png";
	}

	SaveRender(output.c_str());

	ShowProgress();

	// ----- END -----

	m_log.close();
	if (m_useThreads > 1) {
		m_threads.clear();
		m_threadID.clear();
	}

	return true;
}

void PixelRender::RenderTile(const size_t& startIndex) {
}

void PixelRender::Render(const int& minX, const int& minY, const int& maxX, const int& maxY) {
	
}

bool PixelRender::RayHitObject(Ray& ray, const float& t_min, const float& t_max, HitRec& rec) {
	// -- TEMPORARILY EMPTY --
	return false;
}

sf::Vector3f PixelRender::RayColor(Ray& ray, const int& depth, const sf::Vector3f& initialRayCol) {
	return sf::Vector3f();
}

void PixelRender::ShowProgress() {
	Draw();
}
