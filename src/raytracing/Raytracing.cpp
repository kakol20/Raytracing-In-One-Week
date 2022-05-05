#include <chrono>

#include "../misc/Random.h"
#include "../wrapper/Fastwrite.h"
#include "materials/Unshaded.h"
#include "objects/Sphere.h"

#include "Raytracing.h"

Raytracing::Raytracing() {
	m_nextAvailable = 0;
	m_tilesRendered = 0;
	m_useThreads = 1;
	m_xTileCount = 1;
	m_yTileCount = 1;
}

Raytracing::~Raytracing() {
	if (!m_renderedObjects.empty()) {
		for (auto it = m_renderedObjects.begin(); it != m_renderedObjects.end(); it++) {
			delete (*it);
			(*it) = nullptr;
		}
		m_renderedObjects.clear();
	}

	//if (!m_unrenderedObjects.empty()) {
	//	for (auto it = m_unrenderedObjects.begin(); it != m_unrenderedObjects.end(); it++) {
	//		delete (*it).second;
	//		(*it).second = nullptr;
	//	}
	//	m_unrenderedObjects.clear();
	//}

	if (!m_matMap.empty()) {
		for (auto it = m_matMap.begin(); it != m_matMap.end(); it++) {
			delete (*it).second;
			(*it).second = nullptr;
		}
		m_matMap.clear();
	}

	//if (!m_matVec.empty()) {
	//	for (auto it = m_matVec.begin(); it != m_matVec.end(); it++) {
	//		delete (*it);
	//		(*it) = nullptr;
	//	}
	//	m_matVec.clear();
	//}

	//if (!m_textures.empty()) {
	//	for (auto it = m_textures.begin(); it != m_textures.end(); it++) {
	//		delete (*it).second;
	//		(*it).second = nullptr;
	//	}
	//	m_textures.clear();
	//}
}

bool Raytracing::Init() {
	if (!m_settings.Read("settings.cfg")) m_settings.Write("settings.cfg");

	Random::Seed = (unsigned int)std::stoul(m_settings["randomSeed"]);
	if (Random::Seed == 0) Random::Seed = 0xACE1u;

	if (Float::FromString(m_settings["verticalFOV"]) > 90) m_settings["verticalFOV"] = "90";
	if (Float::FromString(m_settings["verticalFOV"]) <= 0) m_settings["verticalFOV"] = "1";

	// generate blue noise samples
	//     only generate if minSamples > 256
	//     generate same number of samples as max samples
	// determine threads to use
	// generate tiles
	// initialise scene

	// ----- MULTI-THREADING -----

	m_useThreads = std::stoi(m_settings["threads"]);
	m_useThreads = m_useThreads <= 0 ? std::thread::hardware_concurrency() : m_useThreads;

	// ----- GENERATE TILES -----

	int imageWidth = std::stoi(m_settings["imageWidth"]);
	int imageHeight = std::stoi(m_settings["imageHeight"]);
	int tileSize = std::stoi(m_settings["tileSize"]);

	m_render = Image(imageWidth, imageHeight, 3, Image::Interpolation::Cubic, Image::Extrapolation::Extend, Image::ColorSpace::sRGB);

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

			m_tiles.push_back({ x, y, maxX, maxY, false, false, Vector3D(1, 0, 0), Vector3D(1, 0, 0), countX, countY, 0 });

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

	// Assign set seed for running raytracing regardless of scene generation - for consistent rendering of samples
	Random::SeedType runSeed = Random::Seed;
	Random::SeedType blueNoiseSeed = (Random::SeedType)Random::RandomUInt();

	// ----- INITIALISE SCENE -----

	FastWrite::Write("Creating Scene \n");

	if (m_settings["scene"] == "original") {
		m_fileFolder = "renders/original/";
		OriginalScene();
	}
	else if (m_settings["scene"] == "debug") {
		m_fileFolder = "renders/debugScene/";
		DebugScene();
	}
	else {
		m_fileFolder = "renders/original/";
		OriginalScene();
	}

	// ----- GENERATE BLUE NOISE -----

	Random::Seed = blueNoiseSeed;
	int minSamples = std::stoi(m_settings["minSamples"]);
	int maxSamples = std::stoi(m_settings["maxSamples"]);

	if (minSamples < 256) {
		int sampleCount = maxSamples < 256 ? maxSamples : 256;
		m_blueNoise.Generate(sampleCount, 10);
	}

	Random::Seed = runSeed;
	for (auto it = m_tiles.begin(); it != m_tiles.end(); it++) {
		(*it).seed = Random::RandomUInt();
	}

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

	if (m_settings["renderMode"] == "color" || m_settings["renderMode"] == "colour") {
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

	if (m_settings["renderMode"] == "color") {
		m_render.SetColorSpace(Image::ColorSpace::sRGB);
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
	else {
		output += "log_color.txt";
	}

	m_log.open(output.c_str(), std::ios_base::out);
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

	// ----- SAVE RENDER -----

	output = m_fileFolder;
	if (m_settings["renderMode"] == "color") {
		output += "render_c.png";
	}

	m_render.Dither();

	if (!m_render.Write(output.c_str())) {
		FastWrite::Reset();
		FastWrite::Write("Error saving : " + output + "\n");

		m_log.close();

		std::cin.ignore();

		return false;
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

void Raytracing::RenderTile(const size_t& startIndex) {
	// set seed
	// start time
	//     Render();
	// end time
	// Assign tile color
	// Log time elapsed

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

	for (int xImg = m_tiles[startIndex].minX; xImg < halfX; xImg++) {
		for (int yImg = m_tiles[startIndex].minY; yImg < m_tiles[startIndex].maxY; yImg++) {
			count += 1.f;
			int flippedY = (imageHeight - yImg) - 1;

			Float r, g, b;
			m_render.GetColor(xImg, flippedY, r, g, b);

			r = Image::LinearToSRGB(r);
			g = Image::LinearToSRGB(g);
			b = Image::LinearToSRGB(b);

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

			r = Image::LinearToSRGB(r);
			g = Image::LinearToSRGB(g);
			b = Image::LinearToSRGB(b);

			getColor += Vector3D(r, g, b);
		}
	}
	getColor /= count;
	m_tiles[startIndex].rightXTileColor = Vector3D::Clamp(getColor, Vector3D::Zero, Vector3D(255));

	m_mutex.lock();
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

	m_mutex.unlock();

	if (next < m_tiles.size()) {
		RenderTile(next);
	}
}

void Raytracing::Render(const int& minX, const int& minY, const int& maxX, const int& maxY) {
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

	if (minSamples > maxSamples) minSamples = maxSamples;

	for (int y = minY; y < maxY; y++) {
		for (int x = minX; x < maxX; x++) {
			int flippedY = (imageHeight - y) - 1;

			// ----- SEND RAYS -----
			Vector3D pixelCol;
			int count = 0;
			Vector3D previous(0);
			Vector3D totalDiff(0);

			size_t samplesOffset = (size_t)Float::Round(Random::RandomFloat(0, (int)m_blueNoise.Size())).ToInt();

			for (int s = 0; s < maxSamples; s++) {
				//Float u = x / Float(imageWidth - 1);
				//Float v = y / Float(imageHeight - 1);

				Float u = x;
				Float v = y;

				if (s < (int)m_blueNoise.Size()) {
					size_t index = s + samplesOffset;
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

				Ray ray = m_camera.GetRay(u, v);
				Vector3D rayColor = RayColor(ray, maxDepth);

				if (count > 0) {
					Vector3D difference;
					difference = previous - (rayColor * 255);
					difference = Vector3D::Abs(difference);
					totalDiff += difference;

					if (count > minSamples) {
						Vector3D avgDiff = totalDiff / count;
						//Vector3D avgDiff = totalDiff;

						//bool belowThreshold = avgDiff.Threshold(m_noiseThreshold);
						bool belowThreshold = avgDiff.Magnitude() < noiseThreshold;

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

			pixelCol /= count;
			pixelCol = Vector3D::Clamp(pixelCol, Vector3D::Zero, Vector3D(1));
			pixelCol *= 255;

			m_render.SetColor(x, flippedY, pixelCol.GetX(), pixelCol.GetY(), pixelCol.GetZ());
		}
	}
}

Vector3D Raytracing::RayColor(Ray& ray, const int& depth) {
	if (depth <= 0) return Vector3D::Zero;

	// check for object hit
	HitRec rec;
	if (RayHitObject(ray, m_clipStart, m_clipEnd, rec)) {
		Ray scattered;
		Vector3D attentuation;

		bool absorb, transparent, emission;

		rec.GetMat()->Scatter(ray, rec, attentuation, scattered, absorb, transparent, emission);

		if (m_settings["renderMode"] == "color") {
			if (absorb || emission) {
				return attentuation;
			}
			else if (transparent) {
				Ray continueRay(rec.GetPoint(), ray.GetDir());
				return RayColor(continueRay, depth - 1);
			}
			else {
				return attentuation * RayColor(scattered, depth - 1);
			}
		}
	}

	// draw background instead if no hit

	Vector3D unitDir = ray.GetDir();
	unitDir.Normalize();
	if (m_settings["renderMode"] == "color") {
		Vector3D uv = unitDir.UVSphere();
		uv -= Vector3D(1, 0, 0);

		Float u = uv.GetX() * m_background.GetWidth();
		Float v = uv.GetY() * m_background.GetHeight();

		Float r, g, b;
		m_background.GetColor(u, v, r, g, b);

		Vector3D rgb(r, g, b);
		rgb /= 255;

		return rgb * m_bgStrength;
	}

	return Vector3D::Zero;
}

bool Raytracing::RayHitObject(Ray& ray, const Float& t_min, const Float& t_max, HitRec& rec) {
	HitRec tempRec;
	bool hit = false;
	Float closest = t_max;

	for (auto it = m_renderedObjects.begin(); it != m_renderedObjects.end(); it++) {
		if ((*it)->Hit(ray, t_min, closest, tempRec)) {
			hit = true;
			closest = tempRec.GetT();
			rec = tempRec;
		}
	}

	return hit;
}

void Raytracing::OriginalScene() {
	// ----- BACKGROUND -----

	m_background = Image(256, 256, 3, Image::Interpolation::Linear, Image::Extrapolation::Extend);
	/*m_background.SetColor(0, 0, 128, Float(0.7) * 255, 255);
	m_background.SetColor(0, 1, 255, 255, 255);*/

	Float bottomY = m_background.GetHeight() / 2;
	Float topY = m_background.GetHeight();

	for (int x = 0; x < m_background.GetWidth(); x++) {
		for (int y = 0; y < m_background.GetHeight(); y++) {
			Vector3D bottom = Vector3D(128, Float(0.7) * 255, 255);
			Vector3D top = Vector3D(255);

			Float r = Float::Map(y, bottomY, topY, bottom.GetX(), top.GetX(), true);
			Float g = Float::Map(y, bottomY, topY, bottom.GetY(), top.GetY(), true);
			Float b = Float::Map(y, bottomY, topY, bottom.GetZ(), top.GetZ(), true);
			m_background.SetColor(x, y, r, g, b);
		}
	}

	m_bgStrength = 1;

	// ----- CAMERA -----

	int imageWidth = std::stoi(m_settings["imageWidth"]);
	int imageHeight = std::stoi(m_settings["imageHeight"]);

	Vector3D lookFrom(13, 2, 3);

	const Float aspectRatio = Float(imageWidth) / imageHeight;
	m_camera = Camera(aspectRatio, Float::FromString(m_settings["blurStrength"]), 10, Float::FromString(m_settings["verticalFOV"]), lookFrom, Vector3D::Zero, Vector3D::Up);

	m_clipStart = 1e-4;
	m_clipEnd = 1000;

	// ----- OBJECTS -----

	// materials

	m_matMap["ground"] = new Unshaded(Vector3D(0.5, 0.5, 0.5));
	m_matMap["rearSphere"] = new Unshaded(Vector3D(0.4, 0.2, 0.1));

	// objects

	m_renderedObjects.push_back(new Sphere(1000, m_matMap["ground"], Vector3D::Zero, Vector3D(0, -1000, 0)));

	m_renderedObjects.push_back(new Sphere(1, m_matMap["rearSphere"], Vector3D::Zero, Vector3D(-4, 1, 0)));
}

void Raytracing::DebugScene() {
	// ----- BACKGROUND -----

	m_background = Image(256, 256, 3, Image::Interpolation::Linear, Image::Extrapolation::Extend);
	/*m_background.SetColor(0, 0, 128, Float(0.7) * 255, 255);
	m_background.SetColor(0, 1, 255, 255, 255);*/

	Float bottomY = m_background.GetHeight() / 2;
	Float topY = m_background.GetHeight();

	for (int x = 0; x < m_background.GetWidth(); x++) {
		for (int y = 0; y < m_background.GetHeight(); y++) {
			Vector3D bottom = Vector3D(128, Float(0.7) * 255, 255);
			Vector3D top = Vector3D(255);

			Float r = Float::Map(y, bottomY, topY, bottom.GetX(), top.GetX(), true);
			Float g = Float::Map(y, bottomY, topY, bottom.GetY(), top.GetY(), true);
			Float b = Float::Map(y, bottomY, topY, bottom.GetZ(), top.GetZ(), true);
			m_background.SetColor(x, y, r, g, b);
		}
	}

	m_bgStrength = 1;

	// ----- CAMERA -----

	int imageWidth = std::stoi(m_settings["imageWidth"]);
	int imageHeight = std::stoi(m_settings["imageHeight"]);

	m_clipStart = 1e-4f;
	m_clipEnd = 1000;

	Vector3D lookFrom(0, 2, 15);
	Vector3D lookAt(0, 1, 0);

	Vector3D dist = lookAt - lookFrom;

	const Float aspectRatio = Float(imageWidth) / imageHeight;
	m_camera = Camera(aspectRatio, 0, dist.Magnitude(), Float::FromString(m_settings["verticalFOV"]), lookFrom, lookAt, Vector3D::Up);

	// ----- OBJECTS -----

	m_matMap["unshaded"] = new Unshaded(Vector3D(1, 0.01, 0.01));

	// objects

	m_renderedObjects.push_back(new Sphere(1, m_matMap["unshaded"], Vector3D::Zero, Vector3D(0, 1, 0)));
}

void Raytracing::ShowProgress() {
	FastWrite::Reset();

	std::string output = "Render Mode: ";
	output += m_settings["renderMode"];
	output += "\nTotal Objects: ";
	output += std::to_string(m_renderedObjects.size());
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
}

void Raytracing::ShuffleTiles() {
	
}