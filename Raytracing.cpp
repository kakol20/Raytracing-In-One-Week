#include <mutex>

#include "oof/oof.h"

#include "FastWrite.h"
#include "Random.h"
#include "String.h"

#include "Raytracing.h"

Raytracing::Raytracing() {
	// ----- SCENE SETTINGS -----
	m_debugScene = false;

	// ----- RENDER SETTINGS -----
	m_aperture = 0.1f;
	m_imageHeight = 720;
	m_imageWidth = 1280;
	m_rayDepth = 12;
	m_shadowDepth = 4;

	m_renderMode = "color";

	m_renderScene = "final";

	m_samplesPerPixel = 128;
	m_tileSize = 32;
	m_verticalFOV = 19.8f;

	m_tilesRendered = 0;
	m_nextAvailable = 0;

	m_useThreads = 6;

	m_nearZero = 1e-8f;
}

bool Raytracing::Init() {
	//Random::Seed = 2790598843;
	Random::Seed = 1 | ((unsigned int)0b1 << 31);

	// ----- SETTINGS FILE -----
	std::fstream settings;
	String consoleOutput;
	consoleOutput += oof::reset_formatting();

	settings.open("settings.cfg", std::ios_base::in);
	if (settings.is_open()) {
		//std::cout << "----- SETTINGS -----\n";
		while (!settings.eof()) {
			String line;
			settings >> line;

			consoleOutput += line.GetChar();
			consoleOutput += '\n';

			String first = line.GetFirst("=");
			//const char* second = line.GetSecond("=");

			if (first == "imageWidth") {
				m_imageWidth = String::ToInt(line.GetSecond("="));
			}
			else if (first == "imageHeight") {
				m_imageHeight = String::ToInt(line.GetSecond("="));
			}
			else if (first == "shadowDepth") {
				m_shadowDepth = String::ToInt(line.GetSecond("="));
			}
			else if (first == "rayDepth") {
				m_rayDepth = String::ToInt(line.GetSecond("="));
			}
			else if (first == "renderMode") {
				m_renderMode = line.GetSecond("=");
			}
			else if (first == "scene") {
				m_renderScene = line.GetSecond("=");
			}
			else if (first == "samples") {
				m_samplesPerPixel = String::ToInt(line.GetSecond("="));
			}
			else if (first == "threads") {
				m_useThreads = (unsigned int)String::ToInt(line.GetSecond("="));

				m_useThreads = m_useThreads <= 0 ? std::thread::hardware_concurrency() : m_useThreads;
			}
			else if (first == "tileSize") {
				m_tileSize = String::ToInt(line.GetSecond("="));
			}
			else if (first == "aperture") {
				m_aperture = String::ToFloat(line.GetSecond("="));
			}
			else if (first == "verticalFOV") {
				m_verticalFOV = String::ToFloat(line.GetSecond("="));
			}
			else if (first == "randomSeed") {
				Random::Seed = String::ToInt(line.GetSecond("="));
			}
		}

		settings.close();
	}
	else {
		// settings file not found
		settings.open("settings.cfg", std::ios_base::out);

		settings << "# Image Settings\n"
			<< "imageWidth=" << m_imageWidth << '\n'
			<< "imageHeight=" << m_imageHeight << "\n#\n"
			<< "# Render Settings\n"
			<< "rayDepth=" << m_rayDepth << '\n'
			<< "shadowDepth=" << m_shadowDepth << '\n'
			<< "threads=" << m_useThreads << '\n'
			<< "samplesPerPixel=" << m_samplesPerPixel << '\n'
			<< "tileSize=" << m_tileSize << '\n'
			<< "## color, normal, albedo or all\n"
			<< "renderMode=" << m_renderMode << '\n'
			<< "## final, textured or debug\n"
			<< "scene=" << m_renderScene << "\n#\n"
			<< "# Camera Settings\n"
			<< "aperture=" << m_aperture << '\n'
			<< "## In degrees\n"
			<< "verticalFOV=" << m_verticalFOV << "\n#\n"
			<< "# Max Seed - " << Random::MaxUInt << '\n'
			<< "randomSeed=" << Random::Seed;

		settings.close();
	}

	// fix zero seed
	if (Random::Seed == 0) {
		Random::Seed = 1 | ((unsigned int)0b1 << 31);
	}

	FastWrite::Write(consoleOutput);
	system("pause");

	// ----- CREATE TILES -----
	int maxXTiles = m_imageWidth / m_tileSize;
	int maxYTiles = m_imageHeight / m_tileSize;
	int maxTiles = maxXTiles * maxYTiles;

	int xTileSize = (int)round(m_imageWidth / (float)maxXTiles);
	int yTileSize = (int)round(m_imageHeight / (float)maxYTiles);

	int widthModulo = m_imageWidth % maxXTiles;
	int heightModulo = m_imageHeight % maxYTiles;

	int x = 0;
	int countX = 0;
	while (x < m_imageWidth) {
		m_xTileCount++;
		int addX = widthModulo > 0 ? xTileSize + 1 : xTileSize;

		int maxX = x + addX;
		maxX = maxX > m_imageWidth ? m_imageWidth : maxX;

		int l_heightModulo = heightModulo;

		int y = 0;
		int yTileCount = 0;
		int countY = 0;
		while (y < m_imageHeight) {
			yTileCount++;
			int addY = l_heightModulo > 0 ? yTileSize + 1 : yTileSize;

			int maxY = y + addY;
			maxY = maxY > m_imageHeight ? m_imageHeight : maxY;

			if (maxX > m_imageWidth || maxY > m_imageHeight) {
				y = y + 0;
			}

			m_tiles.push_back({ x, y, maxX, maxY, false, false, Vector3D(0.f, 0.f, 0.f), Vector3D(0.f, 0.f, 0.f), countX, countY });

			y = maxY;
			l_heightModulo--;
			countY++;
		}

		m_yTileCount = yTileCount;

		x = maxX;
		widthModulo--;
		countX++;
	}

	m_useThreads = m_useThreads < m_tiles.size() ? m_useThreads : m_tiles.size();

	ShowProgress();

	// ----- SCENE INITIALISATION -----

	DebugScene();

	return true;
}

bool Raytracing::Run() {
	String output("");
	output += '\a';

	FastWrite::Write(output);

	//m_hdri.Write("temp/test.png", Image::ColorMode::sRGB);
	if (m_renderMode == "all") {
		// render all modes
	}
	else if (m_renderMode == "color" || m_renderMode == "albedo" || m_renderMode == "normal") {
		return RunMode();
	}
	else {
		m_renderMode = "color";
		return RunMode();
	}

	return true;
}

bool Raytracing::RunMode() {
	for (auto it = m_tiles.begin(); it != m_tiles.end(); it++) {
		(*it).tileComplete = false;
		(*it).activeTile = false;
	}

	ShuffleTiles();

	system("pause"); // temporary
	return true;
}

void Raytracing::ShuffleTiles() {
	size_t i = m_tiles.size() - 1;
	while (i >= 0) {
		size_t swap = (size_t)round(Random::RandFloatRange(0.f, (float)i));

		if (swap < i) {
			std::swap(m_tiles[i], m_tiles[swap]);
		}

		if (i == 0) break;

		i--;
	}
}

void Raytracing::DebugScene() {
	m_hdri.Read("images/hdri/spruit_sunrise_2k.png", Image::ColorMode::sRGB);
}

void Raytracing::FinalScene() {
}

void Raytracing::TexturedScene() {
}

void Raytracing::RenderTile(const size_t startIndex) {
}

void Raytracing::Render(const int minX, const int minY, const int maxX, const int maxY) {
}

void Raytracing::ShowProgress() {
	// ----- OOF -----
	String output = "";
	output += oof::clear_screen();
	output += oof::cursor_visibility(false);
	output += oof::reset_formatting();
	output += oof::bg_color({ 12, 12, 12 });
	output += oof::position(0, 0);

	output += "Render Mode: ";
	output += m_renderMode.GetChar();
	output += "\nThreads Used: ";
	output += std::to_string(m_useThreads);
	output += "\nTotal Tiles: ";
	output += std::to_string(m_tiles.size());
	output += "\nProgress: ";
	output += std::to_string(m_tilesRendered);
	/*output += "\nTile #";

	auto thisId = m_threadID[std::this_thread::get_id()];
	output += std::to_string(thisId);*/

	float progressD = m_tilesRendered / (float)m_tiles.size();
	int total = 23;
	int progressI = (int)floor(progressD * total);

	progressD *= 100.f;
	//progressD = round(progressD * 100.f) / 100.f;

	int startPos = 1;
	output += oof::position(5, 0);
	output += '[';

	for (int x = 0; x < total; x++) {
		float between = (progressD / 100.f) * total;
		//between += (float)startPos;

		if (x < progressI) {
			//std::cout << oof::fg_color({ 255, 255, 255 });
			output += oof::reset_formatting();
		}
		else if (x < between && between < x + 1.f) {
			between = between - x;
			between = std::lerp(12.f, 204.f, between);
			//between *= 255.f;
			between = floor(between);
			//std::cout << oof::fg_color({ (int)between, (int)between, (int)between });
			output += oof::fg_color({ (int)between, (int)between, (int)between });
		}
		else {
			//std::cout << oof::fg_color({ 0, 0, 0 });
			output += oof::fg_color({ 12, 12, 12 });
		}
		// https://www.asciitable.com
		output += (char)254u;
	}
	output += oof::reset_formatting();
	output += "] ";
	output += std::to_string(progressD);
	output += '%';

	// show tile progress
	output += oof::position(6, 0);

	for (auto it = m_tiles.begin(); it != m_tiles.end(); it++) {
		int x = (*it).tileX;
		int y = (*it).tileY;

		for (int i = 0; i <= 1; i++) {
			output += oof::position((m_yTileCount - y) + 6, 2 * x + i);

			if ((*it).tileComplete) {
				Vector3D col;
				if (i == 0) {
					col = (*it).leftXTileColor;
				}
				else {
					col = (*it).rightXTileColor;
				}

				int r = (int)round(col.GetX());
				int g = (int)round(col.GetY());
				int b = (int)round(col.GetZ());
				output += oof::fg_color({ r, g, b });
				output += (char)178u;
			}
			else {
				if ((*it).activeTile) {
					//output += oof::reset_formatting();
					output += oof::fg_color({ 255, 0, 0 });
					output += (char)177u;
				}
				else {
					output += oof::reset_formatting();
					output += (char)176u;
				}
			}
		}
	}

	FastWrite::Write(output);
}

Vector3D Raytracing::RayColor(Ray& ray, const int depth) {
	return Vector3D();
}

const bool Raytracing::RayHitObject(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	return false; // temporary
}

Vector3D Raytracing::ObjectColor(Ray& ray, HitRec& rec, Ray& scattered) {
	return Vector3D(1.f, 1.f, 1.f); // temporary
}

Vector3D Raytracing::ShadowColor(HitRec& rec, const float t_min, const float t_max, const int shadowDepth) {
	return Vector3D(1.f, 1.f, 1.f); // temporary
}
