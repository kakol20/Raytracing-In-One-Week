#define ENABLE_LOGGING
//#define HDR_BACKGROUND

#include <chrono>
#include <filesystem>

#include "../misc/ACES.h"
#include "../misc/Random.h"
#include "../wrapper/Fastwrite.h"
#include "materials/Diffuse.h"
#include "materials/Glass.h"
#include "materials/Metal.h"
#include "materials/Unshaded.h"
#include "objects/PointLight.h"
#include "objects/Sphere.h"
#include "scenes/DebugScene.h"
#include "scenes/FullScene.h"
#include "scenes/OriginalScene.h"

#include "Raytracing.h"

Raytracing::Raytracing() {
	m_nextAvailable = 0;
	m_tilesRendered = 0;
	m_useThreads = 1;
	m_xTileCount = 1;
	m_yTileCount = 1;
	m_scene = nullptr;
}

Raytracing::~Raytracing() {
	delete m_scene;
	m_scene = nullptr;
}

bool Raytracing::Init() {
	if (!m_settings.Read("settings.cfg")) m_settings.Write("settings.cfg");

	Random::Seed = (unsigned int)std::stoul(m_settings["randomSeed"]);
	if (Random::Seed == 0) Random::Seed = 0xACE1u;

	if (Float::FromString(m_settings["verticalFOV"]) > 180) m_settings["verticalFOV"] = "179";
	if (Float::FromString(m_settings["verticalFOV"]) <= 0) m_settings["verticalFOV"] = "1";

	// generate blue noise samples
	//     only generate if minSamples > 256
	//     generate same number of samples as max samples
	// determine threads to use
	// generate tiles
	// initialise scene

	int imageWidth = std::stoi(m_settings["imageWidth"]);
	int imageHeight = std::stoi(m_settings["imageHeight"]);
	int tileSize = std::stoi(m_settings["tileSize"]);

	Float renderScale = Float::FromString(m_settings["renderScale"]);
	imageWidth = (renderScale * imageWidth).ToInt();
	imageHeight = (renderScale * imageHeight).ToInt();
	//tileSize = (renderScale * tileSize).ToInt();

	m_render = Image(imageWidth, imageHeight, 3, Image::Interpolation::Cubic, Image::Extrapolation::Repeat, Image::ColorSpace::sRGB);

	// ----- MULTI-THREADING -----

	m_useThreads = std::stoi(m_settings["threads"]);
	m_useThreads = m_useThreads <= 0 ? std::thread::hardware_concurrency() : m_useThreads;

	// ----- INITIALISE SCENE -----

	FastWrite::Write("Creating Scene \n");

	if (m_settings["scene"] == "debug") {
		m_fileFolder = "renders/debugScene/";
		m_scene = new DebugScene();
		m_scene->Create(m_settings);
	}
	else if (m_settings["scene"] == "original") {
		m_fileFolder = "renders/original/";
		m_scene = new OriginalScene();
		m_scene->Create(m_settings);
	}
	else if (m_settings["scene"] == "full") {
		m_fileFolder = "renders/full/";
		m_scene = new FullScene();
		m_scene->Create(m_settings);
	}
	else {
		m_fileFolder = "renders/debugScene/";
		m_scene = new DebugScene();
		m_scene->Create(m_settings);
	}

	// check if main renders folder exists
	const std::filesystem::path p1{ "renders/" };
	std::filesystem::file_status s1 = std::filesystem::file_status{};
	bool pathExists = std::filesystem::status_known(s1) ? std::filesystem::exists(s1) : std::filesystem::exists(p1);
	if (!pathExists) std::filesystem::create_directory(p1);

	// checks if folder exists
	const std::filesystem::path p2{ m_fileFolder };
	std::filesystem::file_status s2 = std::filesystem::file_status{};

	pathExists = std::filesystem::status_known(s2) ? std::filesystem::exists(s2) : std::filesystem::exists(p2);
	if (!pathExists) std::filesystem::create_directory(p2);

	// ----- GENERATE BLUE NOISE -----

	//Random::Seed = blueNoiseSeed;
	int minSamples = std::stoi(m_settings["minSamples"]);
	int maxSamples = std::stoi(m_settings["maxSamples"]);

	if (minSamples < 256) {
		int sampleCount = maxSamples < 256 ? maxSamples : 256;
		m_blueNoise.Generate(sampleCount, 10);
	}

	// ----- GENERATE TILES -----

	int maxXTiles = imageWidth < tileSize ? 1 : imageWidth / tileSize;
	int maxYTiles = imageHeight < tileSize ? 1 : imageHeight / tileSize;
	int maxTiles = maxXTiles * maxYTiles;

	int xTileSize = Float::Round(Float(imageWidth) / maxXTiles).ToInt();
	int yTileSize = Float::Round(Float(imageHeight) / maxYTiles).ToInt();

	int widthModulo = imageWidth % maxXTiles;
	int heightModulo = imageHeight % maxYTiles;

	int y = 0;
	int countY = 0;
	while (y < imageHeight) {
		m_yTileCount++;

		int addY = heightModulo > 0 ? yTileSize + 1 : yTileSize;
		int maxY = y + addY;
		maxY = maxY > imageHeight ? imageHeight : maxY;

		int l_widthModulo = widthModulo;

		int x = 0;
		int xTileCount = 0;
		int countX = 0;
		while (x < imageWidth) {
			xTileCount++;

			int addX = l_widthModulo > 0 ? xTileSize + 1 : xTileSize;
			int maxX = x + addX;
			maxX = maxX > imageWidth ? imageWidth : maxX;

			m_tiles.push_back({ x, y, maxX, maxY, false, false, Vector3D(1, 0, 0), Vector3D(1, 0, 0), countX, countY, Random::RandomUInt() });

			x = maxX;
			l_widthModulo--;
			countX++;

			//FastWrite::Write(FastWrite::ResetString() + "x: " + std::to_string(x) + "\ny: " + std::to_string(y) + "\n");
		}

		m_xTileCount = xTileCount;

		y = maxY;
		heightModulo--;
		countY++;
	}

	std::reverse(m_tiles.begin(), m_tiles.end());

	return true;
}

bool Raytracing::Run() {
	// start time
	//     RunMode() based on render mode
	// end time
	// log time elapsed

	FastWrite::Reset();
	FastWrite::Write("\a");

	bool success = true;

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
	std::chrono::duration<Float::ForD> elapsedSec = end - start;
	std::chrono::duration<Float::ForD, std::ratio<60, 1>> elapsedMin = end - start;

	std::fstream runTime;
	std::string fileLocation = m_fileFolder;
	fileLocation += "runTime.txt";
	runTime.open(fileLocation.c_str(), std::ios_base::out);
	if (runTime.is_open()) {
		runTime << "Elapsed time in seconds: " << elapsedSec << "\n"
			<< "Elapsed time in minutes: " << elapsedMin << "\n";

		runTime.close();
	}

	return true;
}

bool Raytracing::RunMode() {
	// reset tiles
	// shuffle tiles
	// multi threaded render
	//     RenderTile();
	// save result

	// reset tiles

	if (m_settings["renderMode"] == "color" || m_settings["renderMode"] == "albedo" || m_settings["renderMode"] == "emission") {
		m_render.SetColorSpace(Image::ColorSpace::sRGB);
	}
	else {
		m_render.SetColorSpace(Image::ColorSpace::Non_Color);
	}

	for (auto it = m_tiles.begin(); it != m_tiles.end(); it++) {
		(*it).activeTile = false;
		//(*it).leftXTileColor = Vector3D(1.f, 0.f, 0.f);
		//(*it).rightXTileColor = Vector3D(1.f, 0.f, 0.f);
		(*it).tileComplete = false;
	}

	m_tilesRendered = 0;
	m_nextAvailable = 0;

	ShuffleTiles();

	// ----- RENDER -----

	// logging

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

#ifdef ENABLE_LOGGING
	m_log.open(output.c_str(), std::ios_base::out);
	m_log << "Threads Used: " << m_useThreads << "\nTotal tiles: " << m_tiles.size() << "\n";
#endif // ENABLE_LOGGING

	ShowProgress();

	// ----- MULTI THREADING -----

	m_lastUpdate = std::chrono::high_resolution_clock::now();

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

	int imageWidth = std::stoi(m_settings["imageWidth"]);
	int imageHeight = std::stoi(m_settings["imageHeight"]);
	Image::ColorSpace colorSpace = m_settings["renderMode"] == "normal" ? Image::ColorSpace::Non_Color : Image::ColorSpace::sRGB;
	Image rescaled = Image(imageWidth, imageHeight, 3, Image::Interpolation::Cubic, Image::Extrapolation::Extend, colorSpace);

	FastWrite::Write(FastWrite::ResetString() + "Rescaling Render..\n");
	int renderWidth = m_render.GetWidth();
	int renderHeight = m_render.GetHeight();

	if (m_settings["renderScale"] == "1") {
		if (!m_render.Write(output.c_str())) {
			FastWrite::Reset();
			FastWrite::Write("Error saving : " + output + "\n");

			m_log.close();

			std::cin.ignore();

			return false;
		}
	}
	else {
		for (int x = 0; x < imageWidth; x++) {
			Float x_f = Float(x * renderWidth) / imageWidth;

			for (int y = 0; y < imageHeight; y++) {
				Float y_f = Float(y * renderHeight) / imageHeight;

				Float r, g, b;
				m_render.GetColor(x_f, y_f, r, g, b);

				rescaled.SetColor(x, y, r, g, b);
			}
		}

		//rescaled.Dither();

		if (!rescaled.Write(output.c_str())) {
			FastWrite::Reset();
			FastWrite::Write("Error saving : " + output + "\n");

			m_log.close();

			std::cin.ignore();

			return false;
		}
	}

	// ----- END -----

	FastWrite::Reset();

	m_log.close();
	if (m_useThreads > 1) {
		m_threads.clear();
		m_threadID.clear();
	}

	output = "";
	//output += oof::clear_screen();
	output += "\n\a";

	FastWrite::Write(output);

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	return true;
}

void Raytracing::RenderTile(const size_t startIndex) {
	// set seed
	// start time
	//     Render();
	// end time
	// Assign tile color
	// Log time elapsed

	if (startIndex < m_tiles.size()) {
		m_tiles[startIndex].activeTile = true;
		Random::Seed = m_tiles[startIndex].seed;

		auto start = std::chrono::high_resolution_clock::now();
		Render(m_tiles[startIndex].minX, m_tiles[startIndex].minY, m_tiles[startIndex].maxX, m_tiles[startIndex].maxY);
		auto end = std::chrono::high_resolution_clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		m_tiles[startIndex].activeTile = false;
		std::thread::id thisId = std::this_thread::get_id();
		m_tiles[startIndex].tileComplete = true;

		Vector3D getColor = Vector3D::Zero;
		Float count = 0;
		int halfX = m_tiles[startIndex].maxX + m_tiles[startIndex].minX;
		halfX /= 2;

		int imageHeight = std::stoi(m_settings["imageHeight"]);

		Float renderScale = Float::FromString(m_settings["renderScale"]);
		imageHeight = (renderScale * imageHeight).ToInt();

		for (int xImg = m_tiles[startIndex].minX; xImg < halfX; xImg++) {
			for (int yImg = m_tiles[startIndex].minY; yImg < m_tiles[startIndex].maxY; yImg++) {
				count += 1.f;
				int flippedY = (imageHeight - yImg) - 1;

				Float r, g, b;
				m_render.GetColor(xImg, flippedY, r, g, b);

				/*if (m_settings["renderMode"] != "normal") {
					r = Image::LinearToSRGB(r);
					g = Image::LinearToSRGB(g);
					b = Image::LinearToSRGB(b);
				}*/

				/*r = std::lerp(12.f, 204.f, r / 255.f);
				g = std::lerp(12.f, 204.f, g / 255.f);
				b = std::lerp(12.f, 204.f, b / 255.f);*/

				getColor += Vector3D(r, g, b);
			}
		}

		getColor /= count;
		m_tiles[startIndex].leftXTileColor = Vector3D::Clamp(getColor, Vector3D::Zero, Vector3D(255));

		getColor = Vector3D::Zero;
		count = 0.f;
		for (int xImg = halfX; xImg < m_tiles[startIndex].maxX; xImg++) {
			for (int yImg = m_tiles[startIndex].minY; yImg < m_tiles[startIndex].maxY; yImg++) {
				count += 1.f;
				int flippedY = (imageHeight - yImg) - 1;

				Float r, g, b;
				m_render.GetColor(xImg, flippedY, r, g, b);

				/*r = std::lerp(12.f, 204.f, r / 255.f);
				g = std::lerp(12.f, 204.f, g / 255.f);
				b = std::lerp(12.f, 204.f, b / 255.f);*/

				/*if (m_settings["renderMode"] != "normal") {
					r = Image::LinearToSRGB(r);
					g = Image::LinearToSRGB(g);
					b = Image::LinearToSRGB(b);
				}*/

				getColor += Vector3D(r, g, b);
			}
		}
		getColor /= count;
		m_tiles[startIndex].rightXTileColor = Vector3D::Clamp(getColor, Vector3D::Zero, Vector3D(255));

		m_mutex.lock();
		m_tilesRendered++;

		auto sinceLastUpdate = std::chrono::duration_cast<std::chrono::microseconds>(end - m_lastUpdate);
		if (sinceLastUpdate >= std::chrono::milliseconds(Float(1000. / 12.).ToInt())) {
			m_lastUpdate = end;
			ShowProgress();
		}

		// ----- LOGGING -----
		if (m_useThreads > 1) {
			m_log << "Rendered tile #" << startIndex << " in thread #" << m_threadID[thisId] << " for " << dur << '\n';
		}
		else {
			m_log << "Rendered tile #" << startIndex << " for " << dur << '\n';
		}

		size_t next = m_nextAvailable;
		m_nextAvailable++;

		m_mutex.unlock();

		if (next < m_tiles.size()) {
			RenderTile(next);
		}
	}
}

void Raytracing::Render(const int minX, const int minY, const int maxX, const int maxY) {
	// go through x & y
	// go through samples
	//     adaptive sampling
	//         object intersection RayColor()
	// set image pixels

	Float noiseThreshold = Float::FromString(m_settings["noiseThreshold"]);
	int imageHeight = std::stoi(m_settings["imageHeight"]);
	int imageWidth = std::stoi(m_settings["imageWidth"]);
	int maxDepth = std::stoi(m_settings["rayDepth"]);
	int maxSamples = std::stoi(m_settings["maxSamples"]);
	int minSamples = std::stoi(m_settings["minSamples"]);

	Float renderScale = Float::FromString(m_settings["renderScale"]);
	imageWidth = (renderScale * imageWidth).ToInt();
	imageHeight = (renderScale * imageHeight).ToInt();

	if (minSamples > maxSamples) minSamples = maxSamples;

	for (int y = minY; y < maxY; y++) {
		for (int x = minX; x < maxX; x++) {
			int flippedY = (imageHeight - y) - 1;

			// ----- SEND RAYS -----
			Vector3D pixelCol;
			int count = 0;
			Vector3D previous(0);
			Vector3D totalDiff(0);

			//size_t redOffset = (size_t)Float::Round(Random::RandomFloat(0, (int)m_blueNoise.Size())).ToInt();
			size_t sampleOffset = (size_t)Random::RandomInt(0, (int)m_blueNoise.Size());

			for (int s = 0; s < maxSamples; s++) {
				//Float u = x / Float(imageWidth - 1);
				//Float v = y / Float(imageHeight - 1);

				Float u = x;
				Float v = y;

				if (s < (int)m_blueNoise.Size()) {
					size_t index = s + sampleOffset;
					Vector3D sample = m_blueNoise[index];

					u += sample.GetX();
					v += sample.GetY();
				}
				else {
					u += Random::RandomFloat();
					v += Random::RandomFloat();
				}

				u /= Float(imageWidth - 1);
				v /= Float(imageHeight - 1);

				Vector3D rayColor;

				Ray ray = m_scene->GetCamera().GetRay(u, v);
				rayColor = m_scene->RayColor(ray, maxDepth);
				//rayColor = Vector3D::Clamp(rayColor, Vector3D::Zero, Vector3D(Float::MaxVal, Float::MaxVal, Float::MaxVal));

				if (count > 0) {
					Vector3D difference;
					difference = previous - (rayColor * 255);
					difference = Vector3D::Abs(difference);
					totalDiff += difference;

					if (count > minSamples) {
						Vector3D avgDiff = totalDiff / count;
						//Vector3D avgDiff = totalDiff;

						//bool belowThreshold = avgDiff.Threshold(m_noiseThreshold);
						bool belowThreshold = avgDiff.SqrMagnitude() < noiseThreshold;

						if (belowThreshold) {
							count++;
							pixelCol += rayColor;

							break;
						}
					}
				}

				previous = rayColor * 255;
				count++;
				pixelCol += rayColor;
			}

			if (count <= 0) count = 1;

			pixelCol /= count;
			//pixelCol = Vector3D::Clamp(pixelCol, Vector3D::Zero, Vector3D(1));
			//pixelCol = ACES::ToneMap(pixelCol);
			//Float r = pix

			/*if (m_settings["renderMode"] != "normal") {
				r = Image::LinearToSRGB(r);
				g = Image::LinearToSRGB(g);
				b = Image::LinearToSRGB(b);
			}*/
			
			pixelCol *= 255;

			//m_render.SetColor(x, flippedY, pixelCol.GetX(), pixelCol.GetY(), pixelCol.GetZ());

			if (m_settings["renderMode"] == "color" || m_settings["renderMode"] == "emission") {
				pixelCol /= 255;

				pixelCol = ACES::ToneMap(pixelCol);

				pixelCol *= 255;
			}

			pixelCol = Vector3D::Clamp(pixelCol, Vector3D::Zero, Vector3D::One * 255);

			if (m_settings["renderMode"] != "normal") {
				pixelCol = Vector3D(Image::LinearToSRGB(pixelCol.GetX()), Image::LinearToSRGB(pixelCol.GetY()), Image::LinearToSRGB(pixelCol.GetZ()));
			}

			m_render.SetColor(x, flippedY, pixelCol.GetX(), pixelCol.GetY(), pixelCol.GetZ());

			// c
		}
	}
}

void Raytracing::ShowProgress() {
	//FastWrite::Reset();

#ifdef ENABLE_LOGGING
	std::string output = FastWrite::ResetString();

	output += "Render Mode: ";
	output += m_settings["renderMode"];
	output += "\nTotal Objects: ";
	output += std::to_string(m_scene->ObjectCount());
	output += "\nThreads Used: ";
	output += std::to_string(m_useThreads);
	output += "\nTotal Tiles: ";
	output += std::to_string(m_tiles.size());
	output += "\nProgress: ";
	output += std::to_string(m_tilesRendered);

	Float progressF = m_tilesRendered / Float((int)m_tiles.size());
	int total = 23;
	int progressI = Float::Floor(progressF * total).ToInt();

	progressF *= 100;

	int starPos = 1;
	output += oof::position(6, 0);
	output += '[';

	for (int x = 0; x < total; x++) {
		Float between = (progressF / 100) * total;
		//between += (float)startPos;

		if (x < progressI) {
			//std::cout << oof::fg_color({ 255, 255, 255 });
			output += oof::reset_formatting();
		}
		else if (Float(x) < between && between < Float(x + 1)) {
			between = between - x;
			between = Float::Lerp(12, 204, between);
			between = Float::Floor(between);
			output += oof::fg_color({ between.ToInt(), between.ToInt(), between.ToInt() });
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
	output += Float::ToString(progressF);
	output += '%';

	output += oof::position(6, 0);

	for (auto it = m_tiles.begin(); it != m_tiles.end(); it++) {
		int x = (*it).tileX;
		int y = (*it).tileY;

		for (int i = 0; i < 2; i++) {
			output += oof::position((m_yTileCount - y) + 7, 2 * x + i);

			if ((*it).tileComplete) {
				Vector3D col;
				if (i == 0) {
					col = (*it).leftXTileColor;
				}
				else {
					col = (*it).rightXTileColor;
				}

				/*col /= 255;
				col *= 255;*/

				//int r = std::clamp((int)round(col.GetX()), 0, 255);
				//int g = std::clamp((int)round(col.GetY()), 0, 255);
				//int b = std::clamp((int)round(col.GetZ()), 0, 255);

				int r = Float::Clamp(Float::Round(col.GetX()), 0, 255).ToInt();
				int g = Float::Clamp(Float::Round(col.GetY()), 0, 255).ToInt();
				int b = Float::Clamp(Float::Round(col.GetZ()), 0, 255).ToInt();

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

#else

	FastWrite::Write(FastWrite::ResetString() +
		"Render Mode: " + m_settings["renderMode"] +
		"\nTotal Objects: " + std::to_string(m_renderedObjects.size()) +
		"\nThreads Used: " + std::to_string(m_useThreads) +
		"\nTotal Tiles: " + std::to_string(m_tiles.size()) +
		"\nProgress: " + std::to_string(m_tilesRendered));

#endif // ENABLE_LOGGING
}

void Raytracing::ShuffleTiles() {
	size_t i = m_tiles.size() - 1;
	while (i >= 0) {
		//size_t swap = (size_t)round(Random::RandomFloat(0.f, (float)i));
		size_t swap = (size_t)Random::RandomUInt() % (i + 1);

		if (swap < i) {
			std::swap(m_tiles[i], m_tiles[swap]);
		}

		if (i == 0) break;

		i--;
	}
}