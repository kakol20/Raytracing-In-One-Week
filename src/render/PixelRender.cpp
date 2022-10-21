#include <algorithm>
#include <chrono>
#include <iostream>

#include "../utility/Colour.h"
#include "../utility/Random.h"
#include "../utility/ImageWrapper.h"

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
	m_tilesRendered = 0;

	m_renderColSpce = rt::Image::ColorSpace::Non_Color;
	m_renderScene = false;
}

PixelRender::~PixelRender() {
}

bool PixelRender::Init() {
	m_window.setActive(false);

	m_renderScene = true;

	// ----- SETTINGS -----

	if (!m_settings.Read("settings.cfg")) m_settings.Write("settings.cfg");

	Random::Seed = (unsigned int)std::stoul(m_settings["randomSeed"]);
	if (Random::Seed == 0) Random::Seed = 0xACE1u;

	if (std::stof(m_settings["verticalFOV"]) > 180.f) m_settings["verticalFOV"] = "179";
	if (std::stof(m_settings["verticalFOV"]) <= 0.f) m_settings["verticalFOV"] = "1";

	m_width = std::stoi(m_settings["imageWidth"]);
	m_height = std::stoi(m_settings["imageHeight"]);
	m_useThreads = std::stoi(m_settings["threads"]);

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

		m_fileFolder = "render/debugSc/";

		sf::Vector3f lookFrom = { 0.f, 2.f, 5.f };

		const float aspectRatio = static_cast<float>(m_width) / m_height;

		m_settings["blurStrength"] = "0";
		m_camera = Camera(aspectRatio, std::stof(m_settings["blurStrength"]), 10.f, std::stof(m_settings["verticalFOV"]), lookFrom, rt::Vector3::Zero, rt::Vector3::Up);

		m_clipEnd = 1000.f;

		m_renderImage.create(m_width, m_height, sf::Color::Black);
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

	//std::reverse(m_tiles.begin(), m_tiles.end());

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

	if (m_renderScene) {
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

		m_renderScene = false;
	}
}

bool PixelRender::Draw() {
	sf::Lock lock(m_mutex);

	m_window.setActive(true);

	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			m_window.close();

			//m_mutex.unlock();
			return false;
		}
	}

	//if (sf::Mouse::)

	// ----- DRAWING -----

	m_window.clear();

	m_window.draw(m_renderSprite);

	m_window.display();

	bool isOpen = m_window.isOpen();

	m_window.setActive(false);

	//m_mutex.unlock();

	return isOpen;
}

void PixelRender::SetPixel(const unsigned int& x, const unsigned int& y, const rt::Colour& color) {
	rt::Colour col = color;
	col.Dither(x, y, 255);

	sf::Lock lock(m_mutex);
	m_renderImage.setPixel(x, y, col.GetSFColour());
	//m_mutex.unlock();
}

void PixelRender::UpdateTexture() {
	m_renderTexture.loadFromImage(m_renderImage);
	m_renderSprite.setTexture(m_renderTexture, true);
}

void PixelRender::SaveRender(const char* fileLocation) {
	m_renderImage.saveToFile(fileLocation);
}

bool PixelRender::RunMode() {
	if (m_settings["renderMode"] == "color" || m_settings["renderMode"] == "albedo" || m_settings["renderMode"] == "emission") {
		m_renderColSpce = rt::Image::ColorSpace::sRGB;
	}
	else {
		m_renderColSpce = rt::Image::ColorSpace::Non_Color;
	}

	for (auto it = m_tiles.begin(); it != m_tiles.end(); it++) {
		PixelRender::Tile& tile = (*it);

		tile.activeTile = false;
		tile.tileComplete = false;
	}

	m_tilesRendered = 0;

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

	// ------ MULTI THREADING -----

	if (m_useThreads > 1) {
		for (size_t i = 0; i < m_useThreads; i++) {
			//sf::Thread thread(&PixelRender::RenderTile, this);
			//m_threads.push_back(thread);
			//m_threads[i].launch();

			m_threads.push_back(new sf::Thread(&PixelRender::RenderTile, this));
			m_threads[i]->launch();
		}
	}
	else {
		RenderTile();
	}

	/*if (m_useThreads > 1) {
		for (auto it = m_threads.begin(); it != m_threads.end(); it++) {
			(*it)->launch();
		}
	}*/

	// -- CHECK FOR THREAD FINISH --

	if (m_useThreads > 1) {
		for (auto it = m_threads.begin(); it != m_threads.end(); it++) {
			(*it)->wait();
			//(*it)->terminate();
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

	std::cout << "\nDone rendering \n";

	// ----- END -----

	m_log.close();
	if (m_useThreads > 1) {
		m_threads.clear();
		//m_threadID.clear();
	}

	return true;
}

void PixelRender::RenderTile() {
	// check for next available tile
	sf::Lock lock3(m_mutex);
	size_t startIndex = m_tiles.size();
	for (size_t i = 0; i < m_tiles.size(); i++) {
		if (!m_tiles[i].tileComplete && !m_tiles[i].activeTile) {
			m_tiles[i].activeTile = true;
			startIndex = i;
			break;
		}
	}

	if (startIndex == m_tiles.size()) return;

#ifdef _DEBUG
	std::cout << "Tile #" << startIndex << '\n';
#endif // _DEBUG
	m_mutex.unlock();

	if (startIndex < m_tiles.size()) {
		m_tiles[startIndex].activeTile = true;
		Random::Seed = m_tiles[startIndex].seed;

		auto start = std::chrono::high_resolution_clock::now();
		Render(m_tiles[startIndex].minX, m_tiles[startIndex].minY, m_tiles[startIndex].maxX, m_tiles[startIndex].maxY);
		auto end = std::chrono::high_resolution_clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		m_tiles[startIndex].activeTile = false;
		//std::thread::id thisId = std::this_thread::get_id();
		m_tiles[startIndex].tileComplete = true;

		sf::Lock lock2(m_mutex);
		m_tilesRendered++;

		// ----- LOGGING -----
		/*if (m_useThreads > 1) {
			m_log << "Rendered tile #" << startIndex << " in thread #" << m_threadID[thisId] << " for " << dur << '\n';
		}
		else {
			m_log << "Rendered tile #" << startIndex << " for " << dur << '\n';
		}*/

		m_log << "Rendered tile #" << startIndex << " for " << dur << '\n';

		UpdateTexture();

		if (!Draw()) {
			return;
		}

		m_mutex.unlock();

		RenderTile();
	}
}

void PixelRender::Render(const int& minX, const int& minY, const int& maxX, const int& maxY) {
	sf::Lock lock(m_mutex);

	float noiseThreshold = std::stof(m_settings["noiseThreshold"]);
	int maxDepth = std::stoi(m_settings["rayDepth"]);
	int maxSamples = std::stoi(m_settings["maxSamples"]);
	int minSamples = std::stoi(m_settings["minSamples"]);

	m_mutex.unlock();

	if (minSamples > maxSamples) minSamples = maxSamples;

	for (int x = minX; x < maxX; x++) {
		for (int y = minY; y < maxY; y++) {
			int flippedY = (m_height - y) - 1;

			// ----- SEND RAYS -----

			rt::Colour pixelCol;
			int count = 0;
			rt::Colour previous(0.f, 0.f, 0.f);
			rt::Colour totalDiff(0.f, 0.f, 0.f);

			size_t sampleOffset = static_cast<size_t>(Random::RandomInt(0, static_cast<int>(m_blueNoise.Size())));

			for (int s = 0; s < maxSamples; s++) {
				sf::Vector2f uv = { static_cast<float>(x), static_cast<float>(y) };

				if (s < static_cast<int>(m_blueNoise.Size())) {
					size_t index = s + sampleOffset;

					sf::Vector2f sample = m_blueNoise[index];

					uv += sample;
				}
				else {
					uv += { Random::RandomFloat(), Random::RandomFloat() };
				}

				uv.x /= static_cast<float>(m_width - 1);
				uv.y /= static_cast<float>(m_height - 1);

				rt::Colour rayColour;

				Ray ray = m_camera.GetRay(uv.x, uv.y);

				rayColour = RayColor(ray, maxDepth);

				if (count > 0) {
					rt::Colour difference = previous - rayColour;
					difference.Abs();
					totalDiff += difference;

					if (count > minSamples) {
						rt::Colour avgDiff = totalDiff / static_cast<float>(count);

						rt::Colour::FloatValue fValue = avgDiff.GetFloatValue();
						sf::Vector3f asVec = { fValue.r, fValue.g, fValue.b };

						bool belowThreshold = rt::Vector3::SqrMagnitude(asVec) < (noiseThreshold * noiseThreshold);

						if (belowThreshold) {
							count++;
							pixelCol += rayColour;
							break;
						}
					}
				}

				previous = rayColour;
				count++;
				pixelCol += rayColour;
			}

			if (count <= 0) count = 1;

			pixelCol /= static_cast<float>(count);
			pixelCol.Clamp();

			SetPixel(x, flippedY, pixelCol);
		}
	}
}

bool PixelRender::RayHitObject(Ray& ray, const float& t_min, const float& t_max, HitRec& rec) {
	// -- TEMPORARILY EMPTY --
	return false;
}

rt::Colour PixelRender::RayColor(Ray& ray, const int& depth) {
	if (depth <= 0) return rt::Colour();

	// ----- CHECK FOR OBJECT HIT -----

	HitRec rec;
	if (RayHitObject(ray, m_clipStart, m_clipEnd, rec)) {
		// nothing for now
	}

	// ----- DRAW BACKGROUND -----

	sf::Vector3f unitDir = ray.GetDir();
	unitDir = rt::Vector3::Normalize(unitDir);
	float t = 0.5f * (unitDir.y + 1.f);

	return (rt::Colour(255.f, 255.f, 255.f) * (1.f - t)) + (rt::Colour(0.5f, 0.7f, 1.f) * 255.f * t);
}