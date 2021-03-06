#include <chrono>

#include "oof/oof.h"

#include "Box.h"
#include "ColorSpace.h"
#include "Dielectric.h"
#include "Diffuse.h"
#include "Emissive.h"
#include "FastWrite.h"
#include "Glass.h"
#include "Ground.h"
#include "Metal.h"
#include "Plane.h"
#include "Random.h"
#include "RotatedObj.h"
#include "Sphere.h"
#include "StaticMutex.h"
#include "Textured.h"
#include "TransformedObject.h"
#include "TransformedSphere.h"
#include "TranslatedObj.h"

#include "Raytracing.h"

Raytracing::Raytracing() {
	// ----- RENDER SETTINGS -----
	m_aperture = 0.1f;
	m_imageHeight = 720;
	m_imageWidth = 1280;
	m_rayDepth = 32;
	//m_shadowDepth = 4;

	m_renderMode = "all";

	m_renderScene = "full";

	m_noiseThreshold = 0.0001f;
	m_maxSamples = 512;
	m_minSamples = 0;
	m_tileSize = 32;
	m_verticalFOV = 19.8f;

	m_tilesRendered = 0;
	m_nextAvailable = 0;

	m_useThreads = 12;

	m_nearZero = 1e-4f; // ----- MUST KEEP LIKE THIS (1e-4f) TO PREVENT BLACK SPECKLES -----
	m_clipEnd = 63.27716808f;
	m_clipStart = 1e-4f;

	//m_hdriStrength = 1.f;
	m_hdriStrength = 0.1f;

	//StaticMutex::s_mtx = std::mutex();

	m_shuffleTiles = true;

	//Vector3D testRotation = Vector3D::Rotate(Vector3D(1.f, 0.f, 0.f), Vector3D())
}

Raytracing::~Raytracing() {
	/*std::map<String, Image*> m_textures;
	std::map<String, Material*> m_matMap;
	std::vector<Material*> m_matVec;
	std::vector<Object*> m_objects;
	std::vector<Tile> m_tiles;*/

	if (!m_renderedObjects.empty()) {
		for (auto it = m_renderedObjects.begin(); it != m_renderedObjects.end(); it++) {
			delete (*it);
			(*it) = nullptr;
		}
		m_renderedObjects.clear();
	}

	if (!m_unrenderedObjects.empty()) {
		for (auto it = m_unrenderedObjects.begin(); it != m_unrenderedObjects.end(); it++) {
			delete (*it).second;
			(*it).second = nullptr;
		}
		m_unrenderedObjects.clear();
	}

	if (!m_matMap.empty()) {
		for (auto it = m_matMap.begin(); it != m_matMap.end(); it++) {
			delete (*it).second;
			(*it).second = nullptr;
		}
		m_matMap.clear();
	}

	if (!m_matVec.empty()) {
		for (auto it = m_matVec.begin(); it != m_matVec.end(); it++) {
			delete (*it);
			(*it) = nullptr;
		}
		m_matVec.clear();
	}

	if (!m_textures.empty()) {
		for (auto it = m_textures.begin(); it != m_textures.end(); it++) {
			delete (*it).second;
			(*it).second = nullptr;
		}
		m_textures.clear();
	}
}

bool Raytracing::Init() {
	Random::Seed = 19022022;
	//Random::Seed = 1 | ((unsigned int)0b1 << 31);

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
			consoleOutput += "\n";

			String first = line.GetFirst('=');
			//const char* second = line.GetSecond('=');

			if (first == "imageWidth") {
				m_imageWidth = String::ToInt(line.GetSecond('='));
			}
			else if (first == "imageHeight") {
				m_imageHeight = String::ToInt(line.GetSecond('='));
			}

			//else if (first == "shadowDepth") {
			//	m_shadowDepth = String::ToInt(line.GetSecond('='));
			//}

			else if (first == "rayDepth") {
				m_rayDepth = String::ToInt(line.GetSecond('='));
			}
			else if (first == "renderMode") {
				m_renderMode = line.GetSecond('=');
			}
			else if (first == "scene") {
				m_renderScene = line.GetSecond('=');
			}
			else if (first == "maxSamples") {
				m_maxSamples = String::ToInt(line.GetSecond('='));
			}
			else if (first == "minSamples") {
				m_minSamples = String::ToInt(line.GetSecond('='));
			}
			else if (first == "noiseThreshold") {
				m_noiseThreshold = String::ToFloat(line.GetSecond('='));
			}
			else if (first == "threads") {
				m_useThreads = (unsigned int)String::ToInt(line.GetSecond('='));
			}
			else if (first == "tileSize") {
				m_tileSize = String::ToInt(line.GetSecond('='));
			}
			else if (first == "aperture") {
				m_aperture = String::ToFloat(line.GetSecond('='));
			}
			else if (first == "verticalFOV") {
				m_verticalFOV = String::ToFloat(line.GetSecond('='));
			}
			else if (first == "randomSeed") {
				Random::Seed = String::ToUInt(line.GetSecond('='));
			}
		}

		settings.close();
	}
	else {
		// settings file not found
		settings.open("settings.cfg", std::ios_base::out);

		settings << "# Image Settings\n"
			<< "imageWidth=" << m_imageWidth << "\n"
			<< "imageHeight=" << m_imageHeight << "\n#\n"
			<< "# Render Settings\n"
			<< "rayDepth=" << m_rayDepth << "\n"
			//<< "shadowDepth=" << m_shadowDepth << "\n"
			<< "threads=" << m_useThreads << "\n"
			<< "maxSamples=" << m_maxSamples << "\n"
			<< "minSamples=" << m_minSamples << "\n"
			<< "noiseThreshold=" << m_noiseThreshold << "\n"
			<< "tileSize=" << m_tileSize << "\n"
			<< "## color, normal, albedo, emission or all\n"
			<< "renderMode=" << m_renderMode << "\n"
			<< "## full, textured, cornell, original or debug\n"
			<< "scene=" << m_renderScene << "\n#\n"
			<< "# Camera Settings\n"
			<< "aperture=" << m_aperture << "\n"
			<< "## In degrees\n"
			<< "verticalFOV=" << m_verticalFOV << "\n#\n"
			<< "# Max Seed - " << Random::MaxUInt << "\n"
			<< "randomSeed=" << Random::Seed;

		settings.close();
	}

	// fix zero seed
	if (Random::Seed == 0) {
		Random::Seed = 1 | ((unsigned int)0b1 << 31);
	}

	m_useThreads = m_useThreads <= 0 ? std::thread::hardware_concurrency() : m_useThreads;

	FastWrite::Write(consoleOutput);
	system("pause");

	// ----- CREATE TILES -----
	int maxXTiles = m_imageWidth < m_tileSize ? 1 : m_imageWidth / m_tileSize;
	int maxYTiles = m_imageHeight < m_tileSize ? 1 : m_imageHeight / m_tileSize;
	int maxTiles = maxXTiles * maxYTiles;

	int xTileSize = (int)round(m_imageWidth / (float)maxXTiles);
	int yTileSize = (int)round(m_imageHeight / (float)maxYTiles);

	int widthModulo = m_imageWidth % maxXTiles;
	int heightModulo = m_imageHeight % maxYTiles;

	bool startWithX = false;

	if (startWithX) {
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

				//if (maxX > m_imageWidth || maxY > m_imageHeight) {
				//	y = y + 0;
				//}

				m_tiles.push_back({ x, y, maxX, maxY, false, false, Vector3D(1.f, 0.f, 0.f), Vector3D(1.f, 0.f, 0.f), countX, countY, 0 });

				y = maxY;
				l_heightModulo--;
				countY++;
			}

			m_yTileCount = yTileCount;

			x = maxX;
			widthModulo--;
			countX++;
		}
	}
	else {
		int y = 0;
		int countY = 0;
		while (y < m_imageHeight) {
			m_yTileCount++;

			int addY = heightModulo > 0 ? yTileSize + 1 : yTileSize;
			int maxY = y + addY;
			maxY = maxY > m_imageHeight ? m_imageHeight : maxY;

			int l_widthModulo = widthModulo;

			int x = 0;
			int xTileCount = 0;
			int countX = 0;
			while (x < m_imageWidth) {
				xTileCount++;

				int addX = l_widthModulo > 0 ? xTileSize + 1 : xTileSize;
				int maxX = x + addX;
				maxX = maxX > m_imageWidth ? m_imageWidth : maxX;

				m_tiles.push_back({ x, y, maxX, maxY, false, false, Vector3D(1.f, 0.f, 0.f), Vector3D(1.f, 0.f, 0.f), countX, countY, 0 });

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
	}

	m_useThreads = m_useThreads < m_tiles.size() ? m_useThreads : m_tiles.size();
	m_render = Image(m_imageWidth, m_imageHeight, 3);

	ShowProgress();

	// ----- SCENE INITIALISATION -----

	// Assign set seed for running raytracing regardless of scene generation - for consistent rendering of samples
	unsigned int runSeed = Random::RandomUInt();

	m_fileFolder = "";
	if (m_renderScene == "debug") {
		m_fileFolder = "renders/debug/";
		DebugScene();
	}
	else if (m_renderScene == "textured") {
		m_fileFolder = "renders/textured/";
		TexturedScene();
	}
	else if (m_renderScene == "cornell") {
		m_fileFolder = "renders/cornell/";
		CornellBox();
	}
	else if (m_renderScene == "original") {
		m_fileFolder = "renders/original/";
		OriginalScene();
	}
	else {
		m_fileFolder = "renders/full/";
		FinalScene();
	}

	Random::Seed = runSeed;
	for (auto it = m_tiles.begin(); it != m_tiles.end(); it++) {
		(*it).seed = Random::RandomUInt();
	}

	return true;
}

bool Raytracing::Run() {
	String output("");
	output += "\a";

	FastWrite::Write(output);

	auto start = std::chrono::high_resolution_clock::now();

	bool success = true;

	//m_hdri.Write("temp/test.png", Image::ColorMode::sRGB);
	if (m_renderMode == "all") {
		// render all modes
		m_renderMode = "normal";
		success = RunMode();

		m_renderMode = "emission";
		success = RunMode();

		m_renderMode = "albedo";
		success = RunMode();

		m_renderMode = "color";
		success = RunMode();

		//return success;
	}
	else if (m_renderMode == "color" || m_renderMode == "albedo" || m_renderMode == "normal" || m_renderMode == "emission") {
		success = RunMode();
	}
	else {
		m_renderMode = "color";
		success = RunMode();
	}

	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<float> elapsedSec = end - start;
	std::chrono::duration<float, std::ratio<60, 1>> elapsedMin = end - start;

	std::fstream runTime;
	String fileLocation = m_fileFolder;
	fileLocation += "runTime.txt";
	runTime.open(fileLocation.GetChar(), std::ios_base::out);
	if (runTime.is_open()) {
		runTime << "Elapsed time in seconds: " << elapsedSec << "\n"
			<< "Elapsed time in minutes: " << elapsedMin << "\n";

		runTime.close();
	}

	return success;
}

bool Raytracing::RunMode() {
	for (auto it = m_tiles.begin(); it != m_tiles.end(); it++) {
		(*it).activeTile = false;
		//(*it).leftXTileColor = Vector3D(1.f, 0.f, 0.f);
		//(*it).rightXTileColor = Vector3D(1.f, 0.f, 0.f);
		(*it).tileComplete = false;
	}

	m_tilesRendered = 0;
	m_nextAvailable = 0;

	ShuffleTiles();

	// ---------- RENDER ----------

	// ----- logging -----
	String output = m_fileFolder;
	if (m_renderMode == "albedo") {
		output += "log_albedo.txt";
		m_log.open(output.GetChar(), std::ios_base::out);
	}
	else if (m_renderMode == "emission") {
		output += "log_emission.txt";
		m_log.open(output.GetChar(), std::ios_base::out);
	}
	else if (m_renderMode == "normal") {
		output += "log_normal.txt";
		m_log.open(output.GetChar(), std::ios_base::out);
	}
	else {
		output += "log_color.txt";
		m_log.open(output.GetChar(), std::ios_base::out);
	}

	m_log << "Threads Used: " << m_useThreads << "\nTotal tiles: " << m_tiles.size() << "\n";

	ShowProgress();

	// ----- MULTI THREADING -----
	if (m_useThreads > 1) {
		for (size_t i = 0; i < m_useThreads; i++) {
			m_threads.push_back(std::thread(&Raytracing::RenderTile, this, m_nextAvailable));
			m_threadID[m_threads[i].get_id()] = i;

			//m_threads[i].sleep
			m_nextAvailable++;
		}
	}
	else {
		RenderTile(0);
	}

	ShowProgress();

	// check for threads finish
	if (m_useThreads > 1) {
		for (auto it = m_threads.begin(); it != m_threads.end(); it++) {
			(*it).join();
		}
	}

	//ShowProgress();

	// ----- SAVE RENDER -----
	output = m_fileFolder;
	if (m_renderMode == "albedo") {
		output += "render_a.png";
	}
	else if (m_renderMode == "emission") {
		output += "render_e.png";
	}
	else if (m_renderMode == "normal") {
		output += "render_n.png";
	}
	else {
		output += "render_c.png";
	}
	//m_render.TosRGB();

	if (!m_render.Write(output.GetChar())) {
		std::cout << oof::clear_screen() << oof::reset_formatting() << "Error saving " << output.GetChar() << "\n";

		system("pause");
	}

	// ----- END -----
	m_log.close();
	if (m_useThreads > 1) {
		m_threads.clear();
		m_threadID.clear();
	}

	output = "";
	//output += oof::clear_screen();
	output += oof::reset_formatting();
	output += "\n\a";

	FastWrite::Write(output);

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	//system("pause"); // temporary
	return true;
}

void Raytracing::ShuffleTiles() {
	if (m_shuffleTiles) {
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
}

void Raytracing::CornellBox() {
	m_hdri.Read("images/hdri/lebombo_2k.png", Image::ColorMode::sRGB);
	m_hdriStrength = 0.0255f;
	//m_hdriStrength = 1.f;

	// ----- CAMERA -----
	//Vector3D lookFrom(0.f, 1e-3f, 7.f);
	Vector3D lookFrom(0.f, 0.f, 6.7f);
	Vector3D lookAt(0.f, 0.f);
	Vector3D up(0.f, 1.f, 0.f);

	m_clipEnd = 1000.f;

	Vector3D dist = lookAt - lookFrom;

	const float aspect_ratio = m_imageWidth / (float)m_imageHeight;
	m_camera = Camera(aspect_ratio, m_aperture, dist.Magnitude(), m_verticalFOV, lookFrom, lookAt, up);

	// ----- MATERIAL -----
	float closeToOne = 1.f - m_nearZero;

	m_matMap["light"] = new Emissive(Vector3D::KelvinToRGB(2700.f), 100.f);

	m_matMap["red"] = new Dielectric(Vector3D(0.502887f, 0.f, 0.f), 1.f, 1.45f);
	m_matMap["green"] = new Dielectric(Vector3D(0.f, 0.502887f, 0.f), 1.f, 1.45f);
	m_matMap["blue"] = new Dielectric(Vector3D(0.f, 0.f, 0.502887f), 1.f, 1.45f);
	m_matMap["white"] = new Dielectric(Vector3D(0.401978f), 0.5f, 1.45f);

	m_matMap["glass"] = new Glass(Vector3D(1.f), m_nearZero, 1.5f);
	m_matMap["metal"] = new Metal(Vector3D(0.401978f), 0.25f, 2.95f);

	// ----- OBJECTS -----
	m_renderedObjects.reserve(11);

	m_unrenderedObjects["shortBlock"] = new Box(Vector3D(0.5f), m_matMap["white"]);
	m_unrenderedObjects["tallBlock"] = new Box(Vector3D(0.5f, 1.f, 0.5f), m_matMap["white"]);
	//m_unrenderedObjects["block"] = new Box(Vector3D(1.f), m_matMap["white"]);

	//m_unrenderedObjects["metalBlock"] = new Box(Vector3D(1.f), m_matMap["metal"]);
	m_unrenderedObjects["metalBlock"] = new Box(Vector3D(0.2309f), m_matMap["metal"]);

	//m_unrenderedObjects["glassBlock"] = new Box(Vector3D(1.f), m_matMap["glass"]);
	m_unrenderedObjects["glassBlock1"] = new Box(Vector3D(0.2309f), m_matMap["glass"]);
	m_unrenderedObjects["glassBlock2"] = new Box(Vector3D(0.2309f / 2.f), m_matMap["glass"]);
	//m_unrenderedObjects["glassBlock"] = new Sphere(Vector3D(0.f), 1.f, m_matMap["glass"]);

	m_renderedObjects.push_back(new Plane(Plane::Type::YMinus, Vector3D(0.f, closeToOne, 0.f), 0.45f, 0.45f, m_matMap["light"]));

	m_renderedObjects.push_back(new Plane(Plane::Type::XMinus, Vector3D(1.f, 0.f, 0.f), 2.f, 2.f, m_matMap["green"]));
	m_renderedObjects.push_back(new Plane(Plane::Type::XPlus, Vector3D(-1.f, 0.f, 0.f), 2.f, 2.f, m_matMap["red"]));
	m_renderedObjects.push_back(new Plane(Plane::Type::YMinus, Vector3D(0.f, 1.f, 0.f), 2.f, 2.f, m_matMap["white"]));
	m_renderedObjects.push_back(new Plane(Plane::Type::YPlus, Vector3D(0.f, -1.f, 0.f), 2.f, 2.f, m_matMap["white"]));
	m_renderedObjects.push_back(new Plane(Plane::Type::ZPlus, Vector3D(0.f, 0.f, -1.f), 2.f, 2.f, m_matMap["white"]));

	//m_objects.push_back(new Box(Vector3D(0.5f), m_matMap["blue"]));
	//m_renderedObjects.push_back(new TransformedObject(Vector3D(0.5f), Vector3D(0.f, -16.5f, 0.f), Vector3D(0.33f, -0.75f, 0.43f), m_unrenderedObjects["block"]));
	//m_renderedObjects.push_back(new TransformedObject(Vector3D(1.f), Vector3D(30.f, -16.5f, 30.f), Vector3D(0.33f, -0.75f, 0.43f), m_unrenderedObjects["shortBlock"]));
	m_renderedObjects.push_back(new TransformedObject(Vector3D(1.f), Vector3D(0.f, -16.5f, 0.f), Vector3D(0.33f, -0.75f, 0.43f), m_unrenderedObjects["shortBlock"]));

	//m_renderedObjects.push_back(new TransformedObject(Vector3D(0.5f, 1.f, 0.5f), Vector3D(0.f, 17.6f, 0.f), Vector3D(-0.32f, -0.5f, -0.24f), m_unrenderedObjects["block"]));
	m_renderedObjects.push_back(new TransformedObject(Vector3D(1.f), Vector3D(0.f, 17.6f, 0.f), Vector3D(-0.32f, -0.5f, -0.24f), m_unrenderedObjects["tallBlock"]));

	//m_renderedObjects.push_back(new TransformedObject(Vector3D(0.2309f), Vector3D::Random(0.f, 360.f), Vector3D(0.33f, -0.3f, 0.43f), m_unrenderedObjects["glassBlock"]));
	Vector3D glassBlockRotation = Vector3D::Random(0.f, 360.f);
	m_renderedObjects.push_back(new TransformedObject(false, glassBlockRotation, Vector3D(0.33f, -0.3f, 0.43f), m_unrenderedObjects["glassBlock1"]));
	m_renderedObjects.push_back(new TransformedObject(true, glassBlockRotation, Vector3D(0.33f, -0.3f, 0.43f), m_unrenderedObjects["glassBlock2"]));

	//m_renderedObjects.push_back(new TransformedObject(Vector3D(0.2309f), Vector3D::Random(0.f, 360.f), Vector3D(-0.32f, 0.2f, -0.24f), m_unrenderedObjects["metalBlock"]));
	m_renderedObjects.push_back(new TransformedObject(Vector3D(1.f), Vector3D::Random(0.f, 360.f), Vector3D(-0.32f, 0.2f, -0.24f), m_unrenderedObjects["metalBlock"]));
}

void Raytracing::DebugScene() {
	m_hdri.Read("images/hdri/spruit_sunrise_2k.png", Image::ColorMode::sRGB);
	m_hdriStrength = 1.f;

	// ----- LIGHTS -----
	//m_lights.push_back(Light(Vector3D(-20.f, 15.f, -15.f), Vector3D(1.f, 1.f, 1.f), 5.f, 1165.21671522f));

	// ----- CAMERA -----
	Vector3D lookFrom(0.f, 2.f, 13.f);
	Vector3D lookAt(0.f, 1.f, 0.f);
	Vector3D dist = lookAt - lookFrom;
	Vector3D up(0.f, 1.f, 0.f);

	const float aspect_ratio = m_imageWidth / (float)m_imageHeight;
	m_camera = Camera(aspect_ratio, m_aperture, dist.Magnitude(), m_verticalFOV, lookFrom, lookAt, up);
	//m_camera = Camera(aspect_ratio, 0.f, dist.Magnitude(), m_verticalFOV, lookFrom, lookAt, up);

	// ----- MATERIAL -----
	m_matMap["dielectric"] = new Dielectric(Vector3D::HSVtoRGB(Random::RandFloatRange(0.f, 360.f), 1.f, 1.f), 0.1f, 1.45f);
	m_matMap["diffuse"] = new Diffuse(Vector3D::HSVtoRGB(Random::RandFloatRange(0.f, 360.f), 1.f, 1.f));
	m_matMap["emissive"] = new Emissive(Vector3D::HSVtoRGB(Random::RandFloatRange(0.f, 360.f), Random::RandFloatRange(0.25f, 0.5f), 1.f), 1.f);
	m_matMap["glass"] = new Glass(Vector3D::HSVtoRGB(Random::RandFloatRange(0.f, 360.f), Random::RandFloatRange(0.f, 0.25f), 1.f), 0.f, 1.5f);
	m_matMap["ground"] = new Diffuse(Vector3D(0.8f, 0.8f, 0.8f));
	m_matMap["metal"] = new Metal(Vector3D::HSVtoRGB(Random::RandFloatRange(0.f, 360.f), Random::RandFloatRange(0.5f, 1.f), 1.f), 0.1f, 1.45f);

	// ----- OBJECTS -----
	m_renderedObjects.push_back(new Ground(0.f, m_matMap["ground"]));

	m_renderedObjects.push_back(new Sphere(Vector3D(-2.5f, 1.f, 0.f), 1.f, m_matMap["glass"]));
	m_renderedObjects.push_back(new Sphere(Vector3D(0.f, 1.f, 0.f), 1.f, m_matMap["emissive"]));
	m_renderedObjects.push_back(new Sphere(Vector3D(2.5f, 1.f, 0.f), 1.f, m_matMap["metal"]));

	m_renderedObjects.push_back(new Sphere(Vector3D(-1.25, 0.5f, 1.4143f), 0.5f, m_matMap["diffuse"]));
	m_renderedObjects.push_back(new Sphere(Vector3D(1.25, 0.5f, 1.4143f), 0.5f, m_matMap["dielectric"]));
}

void Raytracing::FinalScene() {
	m_hdri.Read("images/hdri/spruit_sunrise_2k.png", Image::ColorMode::sRGB);

	m_hdriStrength = 0.1f;

	// ----- LIGHTS -----
	//m_lights.push_back(Light(Vector3D(20.f, 15.f, 0.f), Vector3D(1.f, 1.f, 1.f), 10.f, 7853.98163397f));

	// ----- CAMERA -----
	Vector3D lookFrom(13.f, 2.f, 3.f);
	Vector3D lookAt(0.f, 0.f, 0.f);
	Vector3D dist = lookAt - lookFrom;
	Vector3D up(0.f, 1.f, 0.f);
	m_clipEnd = 63.27716808f;

	const float aspectRatio = m_imageWidth / (float)m_imageHeight;
	m_camera = Camera(aspectRatio, m_aperture, dist.Magnitude(), m_verticalFOV, lookFrom, lookAt, up);

	// ----- TEXTURES -----
	m_textures["bricks038_d"] = new Image("images/textures/bricks038/bricks038_d.png", Image::ColorMode::sRGB);
	m_textures["bricks038_n"] = new Image("images/textures/bricks038/bricks038_n.png");
	m_textures["bricks038_rme"] = new Image("images/textures/bricks038/bricks038_rme.png");
	m_textures["fabric004_d"] = new Image("images/textures/fabric004/Fabric004_d.png", Image::ColorMode::sRGB);
	m_textures["fabric004_n"] = new Image("images/textures/fabric004/Fabric004_n.png");
	m_textures["fabric004_rme"] = new Image("images/textures/fabric004/Fabric004_rme.png");
	m_textures["facade020b_d"] = new Image("images/textures/facade020b/facade020b_d.png", Image::ColorMode::sRGB);
	m_textures["facade020b_n"] = new Image("images/textures/facade020b/Facade020B_n.png");
	m_textures["facade020b_rme"] = new Image("images/textures/facade020b/facade020b_rme.png");
	m_textures["ornament_d"] = new Image("images/textures/ornament/ornament_d.png", Image::ColorMode::sRGB);
	m_textures["ornament_n"] = new Image("images/textures/ornament/ornament_n.png");
	m_textures["ornament_rme"] = new Image("images/textures/ornament/ornament_rme.png");
	m_textures["terracotta_d"] = new Image("images/textures/terracotta/terracotta_d.png", Image::ColorMode::sRGB);
	m_textures["terracotta_n"] = new Image("images/textures/terracotta/terracotta_n.png");
	m_textures["terracotta_rme"] = new Image("images/textures/terracotta/terracotta_rme.png");

	// ----- OBJECT CREATION -----
	
	m_renderedObjects.reserve(533);
	m_matVec.reserve(429);

	// materials
	m_matMap["ground"] = new Diffuse(Vector3D(0.5f, 0.5f, 0.5f));
	m_matMap["back"] = new Dielectric(Vector3D(0.4f, 0.2f, 0.1f), 0.1f, 1.46f);
	m_matMap["middle"] = new Glass(Vector3D(1.f, 1.f, 1.f), 0.f, 1.5f);
	m_matMap["front"] = new Metal(Vector3D(0.7f, 0.6f, 0.5f), 0.04f, 0.47f);

	m_matMap["light1"] = new Emissive(Vector3D::KelvinToRGB(5778.f), 3.3f);
	m_matMap["light2"] = new Emissive(Vector3D::KelvinToRGB(5778.f), 1.7f);

	m_matMap["bricks"] = new Textured(m_textures["bricks038_d"], m_textures["bricks038_rme"], m_textures["bricks038_n"], 1.45f);
	m_matMap["carbon"] = new Textured(m_textures["fabric004_d"], m_textures["fabric004_rme"], m_textures["fabric004_n"], 1.45f);
	m_matMap["facade"] = new Textured(m_textures["facade020b_d"], m_textures["facade020b_rme"], m_textures["facade020b_n"], 1.45f, Vector3D(), 1.5f);
	m_matMap["ornament"] = new Textured(m_textures["ornament_d"], m_textures["ornament_rme"], m_textures["ornament_n"], 1.45f);
	m_matMap["terracotta"] = new Textured(m_textures["terracotta_d"], m_textures["terracotta_rme"], m_textures["terracotta_n"], 1.45f);

	// objects
	//m_unrenderedObjects["bricks"] = new Sphere(Vector3D(), 0.2f, m_matMap["bricks"], Vector3D(2.f, 1.f));
	//m_unrenderedObjects["carbon"] = new Sphere(Vector3D(), 0.2f, m_matMap["carbon"], Vector3D(2.f, 1.f));
	//m_unrenderedObjects["facade"] = new Sphere(Vector3D(), 0.2f, m_matMap["facade"], Vector3D(2.f, 1.f));
	//m_unrenderedObjects["ornament"] = new Sphere(Vector3D(), 0.2f, m_matMap["ornament"]);
	//m_unrenderedObjects["terracotta"] = new Sphere(Vector3D(), 0.2f, m_matMap["terracotta"], Vector3D(2.f, 1.f));

	m_renderedObjects.push_back(new Ground(0.f, m_matMap["ground"]));
	m_renderedObjects.push_back(new Sphere(Vector3D(-4.f, 1.f, 0.f), 1.f, m_matMap["back"]));
	m_renderedObjects.push_back(new Sphere(Vector3D(0.f, 1.f, 0.f), 1.f, m_matMap["middle"]));
	m_renderedObjects.push_back(new Sphere(Vector3D(4.f, 1.f, 0.f), 1.f, m_matMap["front"]));

	m_renderedObjects.push_back(new Sphere(Vector3D(20.f, 15.f, -10.f), 4.f, m_matMap["light1"]));
	m_renderedObjects.push_back(new Sphere(Vector3D(20.f, 15.f, 10.f), 8.f, m_matMap["light2"]));

	// procedural
	std::vector<Vector3D> position;
	for (int x = -11; x <= 11; x++) {
		for (int y = -11; y <= 11; y++) {
			Vector3D center((float)x, 0.2f, (float)y);
			Vector3D randPos(Random::RandFloatRange(-0.507107f, 0.507107f), 0.f, Random::RandFloatRange(-0.507107f, 0.507107f));

			center += randPos;

			position.push_back(center);
		}
	}

	for (auto it = position.begin(); it != position.end(); it++) {
		Vector3D position = (*it);

		bool intersect = false;

		for (auto it2 = m_renderedObjects.begin(); it2 != m_renderedObjects.end(); it2++) {
			if ((*it2)->SphereIntersectSphere(position, 0.2f)) {
				intersect = true;
				break;
			}
		}

		if (!intersect) {
			float chooseMat = Random::RandFloat();
			float gap = 1.f / 7.5f;

			if (chooseMat <= 1.f * gap) {
				// ----- DIELECTRIC -----
				float h = Random::RandFloatRange(0.f, 360.f);
				float s = (204.f - 12.f) / 204.f;
				float v = 0.8f;

				float roughness = Random::RandFloat();
				float ior = 1.46f;

				m_matVec.push_back(new Dielectric(Vector3D::HSVtoRGB(h, s, v), roughness, ior));
				m_renderedObjects.push_back(new Sphere(position, 0.2f, m_matVec.back()));
			}
			else if (chooseMat <= 2.f * gap) {
				// ----- METAL -----
				float h = Random::RandFloatRange(0.f, 360.f);
				float s = Random::RandFloatRange(0.25f, 0.5f);
				float v = 1.0f;

				float roughness = Random::RandFloat();
				float ior = 1.45f;

				m_matVec.push_back(new Metal(Vector3D::HSVtoRGB(h, s, v), roughness, ior));
				m_renderedObjects.push_back(new Sphere(position, 0.2f, m_matVec.back()));
			}
			else if (chooseMat <= 3.f * gap) {
				// ----- GLASS -----
				float h = Random::RandFloatRange(0.f, 360.f);
				float s = Random::RandFloatRange(0.f, 0.25f);
				float v = 1.0f;

				float roughness = Random::RandFloatRange(0.f, 0.5f);
				float ior = 1.45f;

				m_matVec.push_back(new Glass(Vector3D::HSVtoRGB(h, s, v), roughness, ior));
				m_renderedObjects.push_back(new Sphere(position, 0.2f, m_matVec.back()));
			}
			else if (chooseMat <= 4.f * gap) {
				// ----- EMISSIVE -----
				float K = Random::RandFloatRange(1000.f, 12200.f);

				//float intensity = 1.f;
				float intensity = Random::RandFloatRange(1.f, 2.f);

				//m_matVec.push_back(new Emissive(Vector3D::HSVtoRGB(h, s, v), intensity));
				m_matVec.push_back(new Emissive(Vector3D::KelvinToRGB(K), intensity));
				m_renderedObjects.push_back(new Sphere(position, 0.2f, m_matVec.back()));
			}
			else {
				// ----- TEXTURED -----
				//float chooseTextured = Random::RandFloat();
				//float texturedGap = 1.f / 4.f;

				Vector3D randomRotation = Vector3D::Random(0.f, 360.f);

				//m_unrenderedObjects["bricks"] = new Sphere(Vector3D(), 0.2f, m_matMap["bricks"], Vector3D(2.f, 1.f));
				//m_unrenderedObjects["carbon"] = new Sphere(Vector3D(), 0.2f, m_matMap["carbon"], Vector3D(2.f, 1.f));
				//m_unrenderedObjects["facade"] = new Sphere(Vector3D(), 0.2f, m_matMap["facade"], Vector3D(2.f, 1.f));
				//m_unrenderedObjects["ornament"] = new Sphere(Vector3D(), 0.2f, m_matMap["ornament"]);
				//m_unrenderedObjects["terracotta"] = new Sphere(Vector3D(), 0.2f, m_matMap["terracotta"], Vector3D(2.f, 1.f));

				if (chooseMat <= 4.5f * gap) {
					//m_renderedObjects.push_back(new TransformedObject(false, randomRotation, position, m_unrenderedObjects["carbon"]));
					m_renderedObjects.push_back(new TransformedSphere(0.2f, m_matMap["carbon"], randomRotation, position, false, Vector3D(2.f, 1.f)));
				}
				else if (chooseMat <= 5.f * gap) {
					//m_renderedObjects.push_back(new TransformedObject(false, randomRotation, position, m_unrenderedObjects["facade"]));
					m_renderedObjects.push_back(new TransformedSphere(0.2f, m_matMap["facade"], randomRotation, position, false, Vector3D(2.f, 1.f)));
				}
				else if (chooseMat <= 6.5f * gap) {
					//m_renderedObjects.push_back(new TransformedObject(false, randomRotation, position, m_unrenderedObjects["ornament"]));
					m_renderedObjects.push_back(new TransformedSphere(0.2f, m_matMap["ornament"], randomRotation, position));
				}
				else if (chooseMat <= 7.f * gap) {
					//m_renderedObjects.push_back(new TransformedObject(false, randomRotation, position, m_unrenderedObjects["bricks"]));
					m_renderedObjects.push_back(new TransformedSphere(0.2f, m_matMap["bricks"], randomRotation, position, false, Vector3D(2.f, 1.f)));
				}
				else {
					//m_renderedObjects.push_back(new TransformedObject(false, randomRotation, position, m_unrenderedObjects["terracotta"]));
					m_renderedObjects.push_back(new TransformedSphere(0.2f, m_matMap["terracotta"], randomRotation, position, false, Vector3D(2.f, 1.f)));
				}
			}
		}
	}
}

void Raytracing::OriginalScene() {
	// ----- BACKGROUND -----

	// ----- CAMERA -----
	Vector3D lookFrom(13.f, 2.f, 3.f);
	Vector3D lookAt(0.f, 0.f, 0.f);
	Vector3D up(0.f, 1.f, 0.f);
	m_clipEnd = 100.f;

	const float aspectRatio = m_imageWidth / (float)m_imageHeight;
	m_camera = Camera(aspectRatio, m_aperture, 10.0, m_verticalFOV, lookFrom, lookAt, up);

	// ----- MATERIALS -----

	m_matMap["ground"] = new Diffuse(Vector3D(0.5f));
	m_matMap["metal"] = new Metal(Vector3D(0.7f, 0.6f, 0.5f), 0.f, 1.45f);
	m_matMap["glass"] = new Glass(Vector3D(1.f), 0.f, 1.5f);
	m_matMap["dielectric"] = new Dielectric(Vector3D(0.4f, 0.2f, 0.1f), 0.f, 1.45f);

	// ----- OBJECTS -----
	m_renderedObjects.reserve(533);
	m_matVec.reserve(429);

	m_renderedObjects.push_back(new Ground(0.f, m_matMap["ground"]));
	m_renderedObjects.push_back(new Sphere(Vector3D(4.f, 1.f, 0.f), 1.f, m_matMap["metal"]));
	m_renderedObjects.push_back(new Sphere(Vector3D(0.f, 1.f, 0.f), 1.f, m_matMap["glass"]));
	m_renderedObjects.push_back(new Sphere(Vector3D(-4.f, 1.f, 0.f), 1.f, m_matMap["dielectric"]));

	// procedural
	std::vector<Vector3D> position;
	for (int x = -11; x <= 11; x++) {
		for (int y = -11; y <= 11; y++) {
			Vector3D center((float)x, 0.2f, (float)y);
			Vector3D randPos(Random::RandFloatRange(0.f, 0.9f), 0.f, Random::RandFloatRange(0.f, 0.9f));

			center += randPos;

			position.push_back(center);
		}
	}
	for (auto it = position.begin(); it != position.end(); it++) {
		Vector3D position = (*it);

		bool intersect = false;

		for (auto it2 = m_renderedObjects.begin(); it2 != m_renderedObjects.end(); it2++) {
			if ((*it2)->SphereIntersectSphere(position, 0.2f)) {
				intersect = true;
				break;
			}
		}

		if (!intersect) {
			float chooseMat = Random::RandFloat();

			if (chooseMat < 0.8f) {
				Vector3D albedo = Vector3D::Random() * Vector3D::Random();
				float roughness = Random::RandFloat();

				m_matVec.push_back(new Dielectric(albedo, roughness, 1.45f));
				m_renderedObjects.push_back(new Sphere(position, 0.2f, m_matVec.back()));
			}
			else if (chooseMat < 0.95f) {
				Vector3D albedo = Vector3D::Random(0.5f, 1.f);
				float roughness = Random::RandFloat() * 0.5f;

				m_matVec.push_back(new Metal(albedo, roughness, 1.45f));
				m_renderedObjects.push_back(new Sphere(position, 0.2f, m_matVec.back()));
			}
			else {
				m_renderedObjects.push_back(new Sphere(position, 0.2f, m_matMap["glass"]));
			}

		}
	}
}

void Raytracing::TexturedScene() {
	m_hdri.Read("images/hdri/spruit_sunrise_2k.png", Image::ColorMode::sRGB);
	m_hdriStrength = 0.1f;
	//m_hdriStrength = 1.f;

	// ----- CAMERA -----
	Vector3D lookFrom(0.f, 2.f, 13.f);
	//Vector3D lookFrom(-13.f, 2.f, 0.f);
	//Vector3D lookFrom(0.f, 9.f, 9.f);
	Vector3D lookAt(0.f, 1.f, 0.f);
	Vector3D dist = lookAt - lookFrom;
	Vector3D up(0.f, 1.f, 0.f);

	const float aspect_ratio = m_imageWidth / (float)m_imageHeight;
	m_camera = Camera(aspect_ratio, m_aperture, dist.Magnitude(), m_verticalFOV, lookFrom, lookAt, up);

	// ----- TEXTURES -----
	m_textures["bricks038_d"] = new Image("images/textures/bricks038/bricks038_d.png", Image::ColorMode::sRGB);
	m_textures["bricks038_n"] = new Image("images/textures/bricks038/bricks038_n.png");
	m_textures["bricks038_rme"] = new Image("images/textures/bricks038/bricks038_rme.png");
	m_textures["fabric004_d"] = new Image("images/textures/fabric004/Fabric004_d.png", Image::ColorMode::sRGB);
	m_textures["fabric004_n"] = new Image("images/textures/fabric004/Fabric004_n.png");
	m_textures["fabric004_rme"] = new Image("images/textures/fabric004/Fabric004_rme.png");
	m_textures["facade020b_d"] = new Image("images/textures/facade020b/facade020b_d.png", Image::ColorMode::sRGB);
	m_textures["facade020b_n"] = new Image("images/textures/facade020b/Facade020B_n.png");
	m_textures["facade020b_rme"] = new Image("images/textures/facade020b/facade020b_rme.png");
	m_textures["ornament_d"] = new Image("images/textures/ornament/ornament_d.png", Image::ColorMode::sRGB);
	m_textures["ornament_n"] = new Image("images/textures/ornament/ornament_n.png");
	m_textures["ornament_rme"] = new Image("images/textures/ornament/ornament_rme.png");
	m_textures["terracotta_d"] = new Image("images/textures/terracotta/terracotta_d.png", Image::ColorMode::sRGB);
	m_textures["terracotta_n"] = new Image("images/textures/terracotta/terracotta_n.png");
	m_textures["terracotta_rme"] = new Image("images/textures/terracotta/terracotta_rme.png", Image::ColorMode::sRGB);

	// ----- MATERIAL -----
	//m_matMap["ground"] = new Diffuse(Vector3D(0.8f, 0.8f, 0.8f));
	m_matMap["bricks"] = new Textured(m_textures["bricks038_d"], m_textures["bricks038_rme"], m_textures["bricks038_n"], 1.45f);
	m_matMap["carbon"] = new Textured(m_textures["fabric004_d"], m_textures["fabric004_rme"], m_textures["fabric004_n"], 1.45f, Vector3D(), 1.f);
	m_matMap["facade"] = new Textured(m_textures["facade020b_d"], m_textures["facade020b_rme"], m_textures["facade020b_n"], 1.45f, Vector3D(), 2.f);
	m_matMap["ornament"] = new Textured(m_textures["ornament_d"], m_textures["ornament_rme"], m_textures["ornament_n"], 1.45f);
	m_matMap["terracotta"] = new Textured(m_textures["terracotta_d"], m_textures["terracotta_rme"], m_textures["terracotta_n"], 1.45f);

	m_matMap["light1"] = new Emissive(Vector3D::KelvinToRGB(2700.f), 10.f);

	// ----- OBJECTS -----
	m_unrenderedObjects["bricks"] = new Sphere(Vector3D(0.f), 1.f, m_matMap["bricks"], Vector3D(2.f, 1.f));
	//m_unrenderedObjects["carbon"] = new Sphere(Vector3D(0.f), 1.f, m_matMap["carbon"], Vector3D(2.f, 1.f));
	m_unrenderedObjects["facade"] = new Box(Vector3D(sqrt(1.f / 3.f)), m_matMap["facade"], Vector3D(1.f));
	m_unrenderedObjects["ornament"] = new Sphere(Vector3D(0.f), 1.f, m_matMap["ornament"]);
	m_unrenderedObjects["terracotta"] = new Sphere(Vector3D(0.f), 0.5f, m_matMap["terracotta"], Vector3D(2.f, 1.f));

	m_renderedObjects.reserve(7);

	m_renderedObjects.push_back(new Ground(0.f, m_matMap["terracotta"], Vector3D(1.f, 1.f, 1.f) / 3.1f));
	m_renderedObjects.push_back(new Sphere(Vector3D(-20.f, 15.f, -15.f), 5.f, m_matMap["light1"]));

	m_renderedObjects.push_back(new TransformedObject(false, Vector3D(0.f), Vector3D(-2.5f, 1.f, 0.f), m_unrenderedObjects["bricks"]));
	//m_renderedObjects.push_back(new TransformedObject(false, Vector3D(0.f), Vector3D(0.f, 1.f, 0.f), m_unrenderedObjects["carbon"]));
	m_renderedObjects.push_back(new TransformedSphere(1.f, m_matMap["carbon"], Vector3D(45.f), Vector3D(0.f, 1.f, 0.f), false, Vector3D(2.f, 1.f)));
	m_renderedObjects.push_back(new TransformedObject(false, Vector3D(0.f), Vector3D(2.5f, 1.f, 0.f), m_unrenderedObjects["ornament"]));

	m_renderedObjects.push_back(new TransformedObject(false, Vector3D(45.f, 45.f, 45.f), Vector3D(-1.25f, 0.5f, 1.5f), m_unrenderedObjects["facade"]));
	//m_renderedObjects.push_back(new TransformedObject(false, Vector3D(45.f, 0.f, 0.f), Vector3D(-1.25f, 0.5f, 1.5f), m_unrenderedObjects["facade"]));
	m_renderedObjects.push_back(new TransformedObject(false, Vector3D(0.f), Vector3D(1.25f, 0.5f, 1.5f), m_unrenderedObjects["terracotta"]));
}

void Raytracing::RenderTile(const size_t startIndex) {
	m_tiles[startIndex].activeTile = true;

	Random::Seed = m_tiles[startIndex].seed;

	/*StaticMutex::s_mtx.lock();
	ShowProgress();
	StaticMutex::s_mtx.unlock();*/

	auto start = std::chrono::high_resolution_clock::now();
	Render(m_tiles[startIndex].minX, m_tiles[startIndex].minY, m_tiles[startIndex].maxX, m_tiles[startIndex].maxY);
	auto end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	m_tiles[startIndex].activeTile = false;

	std::thread::id thisId = std::this_thread::get_id();

	m_tiles[startIndex].tileComplete = true;

	//StaticMutex::s_mtx.lock()

	Vector3D getColor(0.f, 0.f, 0.f);
	float count = 0.f;
	int halfX = m_tiles[startIndex].maxX + m_tiles[startIndex].minX;
	halfX /= 2;

	for (int xImg = m_tiles[startIndex].minX; xImg < halfX; xImg++) {
		for (int yImg = m_tiles[startIndex].minY; yImg < m_tiles[startIndex].maxY; yImg++) {
			count += 1.f;
			int flippedY = (m_imageHeight - yImg) - 1;

			float r, g, b;
			m_render.GetRGB(xImg, flippedY, r, g, b);

			/*r = std::lerp(12.f, 204.f, r / 255.f);
			g = std::lerp(12.f, 204.f, g / 255.f);
			b = std::lerp(12.f, 204.f, b / 255.f);*/

			getColor += Vector3D(r, g, b);
		}
	}
	getColor /= count;
	m_tiles[startIndex].leftXTileColor = Vector3D::Clamp(getColor, 0.f, 255.f);

	getColor = Vector3D(0.f, 0.f, 0.f);
	count = 0.f;
	for (int xImg = halfX; xImg < m_tiles[startIndex].maxX; xImg++) {
		for (int yImg = m_tiles[startIndex].minY; yImg < m_tiles[startIndex].maxY; yImg++) {
			count += 1.f;
			int flippedY = (m_imageHeight - yImg) - 1;

			float r, g, b;
			m_render.GetRGB(xImg, flippedY, r, g, b);

			/*r = std::lerp(12.f, 204.f, r / 255.f);
			g = std::lerp(12.f, 204.f, g / 255.f);
			b = std::lerp(12.f, 204.f, b / 255.f);*/

			getColor += Vector3D(r, g, b);
		}
	}
	getColor /= count;
	m_tiles[startIndex].rightXTileColor = Vector3D::Clamp(getColor, 0.f, 255.f);

	StaticMutex::s_mtx.lock();
	m_tilesRendered++;

	ShowProgress();

	// ----- LOGGING -----
	if (m_useThreads > 1) {
		m_log << "Rendered tile #" << startIndex << " in thread #" << m_threadID[thisId] << " for " << dur << '\n';
	}
	else {
		m_log << "Rendered tile #" << startIndex << " for " << dur << '\n';
	}

	size_t next = m_nextAvailable;
	m_nextAvailable++;

	StaticMutex::s_mtx.unlock();

	if (next < m_tiles.size()) {
		RenderTile(next);
	}
}

void Raytracing::Render(const int minX, const int minY, const int maxX, const int maxY) {
	for (int y = minY; y < maxY; y++) {
		for (int x = minX; x < maxX; x++) {
			int flippedY = (m_imageHeight - y) - 1;

			// ----- SEND RAYS -----
			Vector3D pixelCol;
			int count = 0;
			Vector3D previous(0.f);
			Vector3D totalDiff(0.f);

			for (int s = 0; s < m_maxSamples; s++) {
				float u = (x + Random::RandFloatRange(-1.f, 1.f)) / (float)(m_imageWidth - 1);
				float v = (y + Random::RandFloatRange(-1.f, 1.f)) / (float)(m_imageHeight - 1);

				Ray r = m_camera.GetRay(u, v);
				bool isBackground = false;
				//pixelCol += Vector3D::Clamp(RayColor(r, m_rayDepth), 0.f, 1.f);
				Vector3D rayColor = RayColor(r, m_rayDepth, isBackground);
				if (rayColor.IsNAN()) rayColor = Vector3D(1.f, 0.f, 1.f);
				//if (rayColor.NearZero()) rayColor = Vector3D(1.f, 0.f, 1.f);
				//pixelCol += RayColor(r, m_rayDepth);

				if (m_renderMode == "emission" || m_renderMode == "albedo") {
					rayColor = Vector3D::Clamp(rayColor, 0.f, 1.f);
				}

				if (m_renderMode == "normal") {
					//rayColor.Normalize();
					rayColor = (rayColor + Vector3D(1.f)) / 2.f;
				}

				//rayColor = Vector3D::OrderedDithering(rayColor, (int)round(u), (int)round(v), 255);

				if (count > 0) {
					Vector3D difference;
					difference = previous - (rayColor * 255.f);
					difference.Abs();
					totalDiff += difference;

					if (count > m_minSamples) {
						Vector3D avgDiff = totalDiff / (float)count;
						//Vector3D avgDiff = totalDiff;

						bool belowThreshold = avgDiff.Threshold(m_noiseThreshold);
						//bool belowThreshold = avgDiff.Magnitude() < m_noiseThreshold;

						if (belowThreshold) {
							count++;
							pixelCol += rayColor;

							break;
						}
					}
				}

				previous = rayColor * 255.f;

				count++;
				pixelCol += rayColor;
			}
			//float scale = 1.f / ;

			pixelCol /= (float)count;
			pixelCol = Vector3D::Clamp(pixelCol, 0.f, 1.f);

			if (m_renderMode == "albedo" || m_renderMode == "color" || m_renderMode == "emission") {
				pixelCol = ColorSpace::LinearTosRGB(pixelCol);
			}

			//else if () {
			//	//pixelCol = ColorSpace::CIEXYZToACES(ColorSpace::LinearToCIEXYZ(pixelCol));
			//	pixelCol = ColorSpace::LinearTosRGB(pixelCol);
			//}

			//pixelCol = Vector3D::Clamp(pixelCol, 0.f, 1.f);

			if (pixelCol.IsNAN()) {
				pixelCol = Vector3D(1.f, 0.f, 1.f);
			}

			// ----- WRITE COLOR -----
			pixelCol = Vector3D::OrderedDithering(pixelCol, x, flippedY, 255);
			pixelCol *= 255.f;

			m_render.SetRGB(x, flippedY, pixelCol.GetX(), pixelCol.GetY(), pixelCol.GetZ());
		}
	}
}

void Raytracing::ShowProgress() {
	// ----- OOF -----
	//output = "";

	//std::string output = "";
	String output = "";
	output += oof::clear_screen();
	output += oof::cursor_visibility(false);
	output += oof::reset_formatting();
	output += oof::bg_color({ 12, 12, 12 });
	output += oof::position(0, 0);

	output += "Render Mode: ";
	output += m_renderMode;
	output += "\nTotal Objects: ";
	output += String::ToString((int)m_renderedObjects.size());
	output += "\nThreads Used: ";
	output += String::ToString((int)m_useThreads);
	output += "\nTotal Tiles: ";
	output += String::ToString((int)m_tiles.size());
	output += "\nProgress: ";
	output += String::ToString(m_tilesRendered);
	/*output += "\nTile #";

	auto thisId = m_threadID[std::this_thread::get_id()];
	output += String::ToString(thisId);*/

	float progressF = m_tilesRendered / (float)m_tiles.size();
	int total = 23;
	int progressI = (int)floor(progressF * total);

	progressF *= 100.f;
	//progressD = round(progressD * 100.f) / 100.f;

	int startPos = 1;
	output += oof::position(6, 0);
	output += "[";

	for (int x = 0; x < total; x++) {
		float between = (progressF / 100.f) * total;
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

		//char end = '\\';
	}
	output += oof::reset_formatting();
	output += "] ";
	output += String::ToString(progressF).GetChar();
	output += "%";

	// show tile progress
	output += oof::position(6, 0);

	for (auto it = m_tiles.begin(); it != m_tiles.end(); it++) {
		int x = (*it).tileX;
		int y = (*it).tileY;

		for (int i = 0; i <= 1; i++) {
			output += oof::position((m_yTileCount - y) + 7, 2 * x + i);

			if ((*it).tileComplete) {
				Vector3D col;
				if (i == 0) {
					col = (*it).leftXTileColor;
				}
				else {
					col = (*it).rightXTileColor;
				}

				col /= 255.f;
				col = Vector3D::OrderedDithering(col, 2 * x + i, m_yTileCount - y, 255);
				col *= 255.f;

				int r = std::clamp((int)round(col.GetX()), 0, 255);
				int g = std::clamp((int)round(col.GetY()), 0, 255);
				int b = std::clamp((int)round(col.GetZ()), 0, 255);
				output += oof::fg_color({ r, g, b });
				//output += (char)219u;
				output += (char)178u;
			}
			else {
				if ((*it).activeTile) {
					//output += oof::reset_formatting();
					output += oof::fg_color({ 204, 12, 12 });
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

Vector3D Raytracing::RayColor(Ray& ray, const int depth, bool& isBackground) {
	// If we"ve exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0) return Vector3D(m_nearZero);

	//float clipEnd = 1000.f;

	HitRec rec;
	if (RayHitObject(ray, m_clipStart, m_clipEnd, rec)) {
		Ray scattered;
		bool continueRay = false;
		bool alpha = false;

		isBackground = false;

		if (m_renderMode == "albedo") {
			if (!alpha) {
				Vector3D emission = EmissionColor(rec);
				if (rec.GetMat()->Emission(rec, emission)) {
					Vector3D emission = EmissionColor(rec);
					return emission;
				}
				else {
					Vector3D objCol = ObjectColor(ray, rec, scattered, continueRay, alpha);
					return objCol;
				}
			}
			else {
				scattered = Ray(rec.GetPoint(), ray.GetDir());
				return RayColor(scattered, depth - 1, isBackground);
			}
		}
		else if (m_renderMode == "emission") {
			Vector3D emission = EmissionColor(rec);
			return emission;
		}
		else if (m_renderMode == "normal") {
			Vector3D normal = rec.GetMat()->GetNormal(rec);
			//Vector3D normal = rec.GetNormal();
			normal.Normalize();
			return normal;
		}
		else {
			//Vector3D emissionCol = EmissionColor(rec);
			Vector3D objCol = ObjectColor(ray, rec, scattered, continueRay, alpha);
			Vector3D emission = EmissionColor(rec);

			if (continueRay) {
				return objCol * RayColor(scattered, depth - 1, isBackground);
			}
			else {
				return emission;
			}
		}
	}

	Vector3D unitDir = ray.GetDir();
	unitDir.Normalize();

	isBackground = true;

	if (m_renderMode == "normal") {
		//return ((unitDir * -1.f) + Vector3D(1.f, 1.f, 1.f)) / 2.f;
		return -unitDir;
		//return Vector3D(-1.f);
	}
	else if (m_renderMode == "emission") {
		return Vector3D();
	}
	else {
		if (m_renderScene == "original") {
			float t = 0.5f * (unitDir.GetY() + 1.f);
			return (1.f - t) * Vector3D(1.f) + t * Vector3D(0.5f, 0.7f, 1.f);
		}
		else {
			float u, v;
			unitDir.UVSphere(u, v);
			//u = 1.f - u;
			u -= 1.f;

			u *= (float)(m_hdri.GetWidth());
			v *= (float)(m_hdri.GetHeight());

			float r, g, b;
			m_hdri.BiLerp(u, v, r, g, b);

			Vector3D rgb(r, g, b);
			rgb /= 255.f;

			//m_hdriStrength = 0.1f;
			return rgb * m_hdriStrength;
		}
	}
}

const bool Raytracing::RayHitObject(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	HitRec tempRec;
	bool hit = false;
	float closest = t_max;

	for (auto it = m_renderedObjects.begin(); it != m_renderedObjects.end(); it++) {
		if ((*it)->Hit(ray, t_min, closest, tempRec)) {
			hit = true;
			closest = tempRec.GetT();
			rec = tempRec;
		}
	}
	return hit;
}

Vector3D Raytracing::EmissionColor(HitRec& rec) {
	// ----- EMISSIVE OBJECT -----
	Vector3D emission;
	if (rec.GetMat()->Emission(rec, emission)) return emission;

	return Vector3D(0.f);
}

Vector3D Raytracing::ObjectColor(Ray& ray, HitRec& rec, Ray& scattered, bool& continueRay, bool& alpha) {
	Vector3D attentuation;
	if (rec.GetMat()->Scatter(ray, rec, attentuation, scattered)) {
		continueRay = true;
		alpha = false;

		return attentuation;
	}
	else {
		continueRay = false;
		alpha = false;

		//scattered =
		scattered = Ray(rec.GetPoint(), ray.GetDir());

		return attentuation;
	}
}