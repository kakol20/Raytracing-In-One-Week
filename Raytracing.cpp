#include <algorithm>
#include <ctime>
#include <iostream>

#include "Dielectric.h"
#include "Filters.h"
#include "Glass.h"
#include "Ground.h"
#include "Lambertian.h"
#include "LinearFeedbackShift.h"
#include "Metal.h"
#include "Raytracing.h"
#include "Sphere.h"
#include "StaticMutex.h"

bool Image::PrintToConsole = false;

Raytracing::Raytracing() {
	// default settings
	m_imageWidth = 1280;
	m_imageHeight = 720;

	// Other
	m_samplesPerPixel = 128;
	m_maxDepth = 12;
	m_tileSize = 32;

	m_aperture = 0.1f;
	m_verticalFOV = 19.8f;

	m_nextAvailable = 0;

	m_renderMode = "all";

	if (m_renderMode == "normal") {
		m_renderNormals = true;
		m_renderAlbedo = false;
	}
	else if (m_renderMode == "albedo") {
		m_renderNormals = false;
		m_renderAlbedo = true;
	}
	else if (m_renderMode == "color" || m_renderMode == "colour") {
		m_renderNormals = false;
		m_renderAlbedo = false;
	}
	else {
		m_renderNormals = false;
		m_renderAlbedo = false;
	}

	//m_mainLight = Light();

	m_hdri.Read("images/cloud_layers_4k.png");

	m_hdriResolution = 0.5f;

	m_debugMode = false;
}

void Raytracing::Init() {
	// settings
	// get settings.cfg
	std::fstream settings;

	settings.open("settings.cfg", std::ios_base::in);

	if (settings.is_open()) {
		// settings.cfg found
		while (!settings.eof()) {
			String line;
			settings >> line;

			std::cout << line;
			std::cout << '\n';

			//std::cout << line.GetFirst("=") << " = " << String::ToInt(second.GetChar()) << "\n\n";
			String first = line.GetFirst("=");
			String second = line.GetSecond("=");

			if (first == "aperture") {
				// float
				m_aperture = String::ToFloat(second.GetChar());
			}
			else if (first == "imageHeight") {
				m_imageHeight = String::ToInt(second.GetChar());
			}
			else if (first == "imageWidth") {
				m_imageWidth = String::ToInt(second.GetChar());
			}
			/*else if (first == "hdriResolution") {
				m_hdriResolution = String::ToFloat(second.GetChar());
			}*/
			else if (first == "maxDepth") {
				m_maxDepth = String::ToInt(second.GetChar());
			}
			else if (first == "renderMode") {
				m_renderMode = second.GetChar();

				if (m_renderMode == "normal") {
					m_renderNormals = true;
					m_renderAlbedo = false;
				}
				else if (m_renderMode == "albedo") {
					m_renderNormals = false;
					m_renderAlbedo = true;
				}
				else if (m_renderMode == "color" || m_renderMode == "colour") {
					m_renderNormals = false;
					m_renderAlbedo = false;
				}
				else {
					m_renderNormals = false;
					m_renderAlbedo = false;
				}
			}
			else if (first == "randomSeed") {
				LinearFeedbackShift::Seed = (unsigned int)String::ToInt(second.GetChar());
			}
			else if (first == "samplesPerPixel") {
				m_samplesPerPixel = String::ToInt(second.GetChar());
			}
			else if (first == "tileSize") {
				m_tileSize = String::ToInt(second.GetChar());
			}
			else if (first == "verticalFOV") {
				// float
				m_verticalFOV = String::ToFloat(second.GetChar());
			}
		}

		//system("pause");

		std::cout << '\n';

		settings.close();
		//
	}
	else {
		// create settings.cfg
		settings.close();

		settings.open("settings.cfg", std::ios_base::out);
		settings << "#Image Settings\n";
		settings << "imageWidth=" << m_imageWidth << '\n';
		settings << "imageHeight=" << m_imageHeight << "\n#\n";

		settings << "#Render Settings\n";
		settings << "##0 to 1\n";
		//settings << "hdriResolution=" << m_hdriResolution << '\n';
		settings << "maxDepth=" << m_maxDepth << '\n';
		settings << "##color, normal, albedo or all\n";
		settings << "renderMode=" << m_renderMode << '\n';
		settings << "samplesPerPixel=" << m_samplesPerPixel << '\n';
		settings << "tileSize=" << m_tileSize << "\n#\n";

		settings << "#Camera Settings\n";
		settings << "aperture=" << m_aperture << '\n';
		settings << "##In degrees\n";
		settings << "verticalFOV=" << m_verticalFOV << "\n#\n";

		settings << "#Max Seed Value " << std::numeric_limits<unsigned int>::max() << "\n";
		settings << "randomSeed=" << LinearFeedbackShift::Seed;

		settings.close();
	}

	// Image
	m_render = Image(m_imageWidth, m_imageHeight, 3);
	const float aspect_ratio = m_imageWidth / (float)m_imageHeight;

	// Lights
	bool oneOther = true;
	if (oneOther) {
		// light #1
		Vector3D lightPos = Vector3D(158.0f, 242.0f, 81.0f) / 255.0f;
		lightPos = (lightPos * 2.0f) - Vector3D(1.0f, 1.0f, 1.0f);
		//Vector3D lightPos = Vector3D(0.5f, 1.0f, 0.5f);
		lightPos = lightPos.UnitVector();

		float u, v;
		UVSphere(lightPos, u, v);
		lightPos *= 1000.0f;

		Vector3D lightColor = BiLerp(u, v, m_hdri);
		m_lights.push_back(Light(lightPos, lightColor, 1000.0f));

		// Light(Vector3D(20.0f, 15.0f, 0.0f), Vector3D(1.0f, 1.0f, 1.0f));

		// light #2
		lightPos = Vector3D(20.0f, 15.0f, 0.0f);
		//lightPos = lightPos.UnitVector();

		UVSphere(lightPos.UnitVector(), u, v);
		//lightPos *= 25.0f;

		lightColor = BiLerp(u, v, m_hdri);
		m_lights.push_back(Light(lightPos, lightColor, 1000.0f));

	}

	//float step = 2.0f * m_hdriResolution;
	//for (float x = -1.0f; x <= 1.0f; x += step) {
	//	for (float z = -1.0f; z <= 1.0f; z += step) {
	//		Vector3D lightPos = Vector3D(158.0f, 242.0f, 81.0f) / 255.0f;
	//		lightPos = (lightPos * 2.0f) - Vector3D(1.0f, 1.0f, 1.0f);
	//		//Vector3D lightPos = Vector3D(0.5f, 1.0f, 0.5f);

	//		lightPos = lightPos * Vector3D(x, 1.0f, z);
	//		lightPos = lightPos.UnitVector();

	//		float u, v;
	//		UVSphere(lightPos, u, v);
	//		lightPos *= 25.0f;

	//		Vector3D lightColor = BiLerp(u, v, m_hdri);
	//		//lightColor = Vector3D(sqrtf(lightColor.GetX()), sqrtf(lightColor.GetY()), sqrtf(lightColor.GetZ()));

	//		m_lights.push_back(Light(lightPos, lightColor, 1000.0f));
	//	}
	//}

	Vector3D up(0.0f, 1.0f, 0.0f);
	if (!m_debugMode) {
		// Camera
		Vector3D lookFrom(13.0f, 2.0f, 3.0f);
		Vector3D lookAt(0.0f, 0.0f, 0.0f);
		Vector3D dist = Vector3D(4.0f, 1.0f, 0.0f) - lookFrom;

		m_camera = Camera(aspect_ratio, m_aperture, 10.0f, m_verticalFOV, lookFrom, lookAt, up); // 39.6 deg fov for 50mm focal length

		//m_mainLight = Light(lightPos, lightColor);

		// Create Materials
		m_materials["glass"] = new Glass(Vector3D(1.0f, 1.0f, 1.0f), 0.0f, 1.5f);
		m_materials["diffuse"] = new Dielectric(Vector3D(0.4f, 0.2f, 0.1f), 0.1f, 1.5f);
		m_materials["metal"] = new Metal(Vector3D(0.7f, 0.6f, 0.5f), 0.25f, 1.5f);
		m_materials["ground"] = new Lambertian(Vector3D(0.5f, 0.5f, 0.5f), 1.5f);

		// Create Objects
		//m_objects.push_back(new Sphere(Vector3D(0.0f, 1.0f, 0.0f), -0.95f, m_materials["glass"]));
		m_objects.push_back(new Sphere(Vector3D(0.0f, 1.0f, 0.0f), 1.0f, m_materials["glass"]));
		m_objects.push_back(new Sphere(Vector3D(-4.0f, 1.0f, 0.0f), 1.0f, m_materials["diffuse"]));
		m_objects.push_back(new Sphere(Vector3D(4.0f, 1.0f, 0.0f), 1.0f, m_materials["metal"]));
		//m_objects.push_back(new Sphere(Vector3D(0.0f, -1000.0f, 0.0f), 1000.0f, m_materials["ground"]));
		m_objects.push_back(new Ground(0.0f, m_materials["ground"]));

		unsigned int bitCount = 32;

		// Procedural Objects
		int index = 0;
		for (int a = -10; a <= 10; a++) {
			for (int b = -10; b <= 10; b++) {
				float chooseMat = LinearFeedbackShift::RandFloat(bitCount);
				//Vector3D center((float)a + 0.9f * LinearFeedbackShift::RandFloat(bitCount), 0.2f, (float)b + 0.9f * LinearFeedbackShift::RandFloat(bitCount));
				Vector3D center((float)a, 0.2f, (float)b);
				Vector3D rand = Vector3D(0.9f, 0.9f, 0.9f) * Vector3D::Random(0.0f, 1.0f, bitCount) * Vector3D(1.0f, 0.0f, 1.0f);

				center = center + rand;

				Vector3D dist2 = center - Vector3D(4.0f, 2.0f, 0.0f);

				if (dist2.Magnitude() > 0.9f) {
					if (chooseMat < 0.2f) {
						// lambertian
						// generate colour based on hue
						float h = LinearFeedbackShift::RandFloatRange(0.0f, 360.0f, bitCount);
						float s = 1.0f;
						float v = 220.0f / 255.0f;

						float c = v * s;
						float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2) - 1.0f));
						float m = v - c;

						float r = 0.0f, g = 0.0f, b = 0.0f;
						if (h < 60.0f) {
							r = c;
							g = x;
						}
						else if (h < 120.0f) {
							r = x;
							g = c;
						}
						else if (h < 180.0f) {
							g = c;
							b = x;
						}
						else if (h < 240.0f) {
							g = x;
							b = c;
						}
						else if (h < 300.0f) {
							r = x;
							b = c;
						}
						else {
							r = c;
							b = x;
						}

						r += m;
						g += m;
						b += m;

						m_proceduralMats.push_back(new Lambertian(Vector3D(r, g, b), 1.45f));

						m_objects.push_back(new Sphere(center, 0.2f, m_proceduralMats[index]));
					}
					else  if (chooseMat < 0.8f) {
						// dielectric
						// generate colour based on hue
						float h = LinearFeedbackShift::RandFloatRange(0.0f, 360.0f, bitCount);
						float s = 1.0f;
						float v = 220.0f / 255.0f;

						float c = v * s;
						float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2) - 1.0f));
						float m = v - c;

						float r = 0.0f, g = 0.0f, b = 0.0f;
						if (h < 60.0f) {
							r = c;
							g = x;
						}
						else if (h < 120.0f) {
							r = x;
							g = c;
						}
						else if (h < 180.0f) {
							g = c;
							b = x;
						}
						else if (h < 240.0f) {
							g = x;
							b = c;
						}
						else if (h < 300.0f) {
							r = x;
							b = c;
						}
						else {
							r = c;
							b = x;
						}

						r += m;
						g += m;
						b += m;
						float roughness = LinearFeedbackShift::RandFloatRange(0.0f, 0.5f, bitCount);
						float ior = 1.45f;

						m_proceduralMats.push_back(new Dielectric(Vector3D(r, g, b), roughness, ior));

						m_objects.push_back(new Sphere(center, 0.2f, m_proceduralMats[index]));
					}
					else if (chooseMat < 0.9f) {
						// metal
						Vector3D albedo = Vector3D::Random(0.5f, 1.0f, bitCount);
						float roughness = LinearFeedbackShift::RandFloatRange(0.0f, 1.0f, bitCount);
						float ior = 1.45f;
						m_proceduralMats.push_back(new Metal(albedo, roughness, ior));

						m_objects.push_back(new Sphere(center, 0.2f, m_proceduralMats[index]));
					}
					else {
						// glass
						Vector3D albedo = Vector3D::Random(0.5f, 1.0f, bitCount);
						float roughness = LinearFeedbackShift::RandFloatRange(0.0f, 0.5f, bitCount);
						float ior = 1.5f;
						m_proceduralMats.push_back(new Glass(albedo, roughness, ior));
						m_objects.push_back(new Sphere(center, 0.2f, m_proceduralMats[index]));
					}
					index++;
				}
			}
		}

		for (auto it = m_materials.begin(); it != m_materials.end(); it++) {
			(*it).second->CameraPos(lookFrom);
		}
		for (auto it = m_proceduralMats.begin(); it != m_proceduralMats.end(); it++) {
			(*it)->CameraPos(lookFrom);
		}
	}
	else {
		// Camera
		Vector3D lookFrom(13.0f, 2.0f, 0.0f);
		Vector3D lookAt(0.0f, 1.0f, 0.0f);
		Vector3D dist = lookAt - lookFrom;
		m_camera = Camera(aspect_ratio, m_aperture, dist.Magnitude(), m_verticalFOV, lookFrom, lookAt, up); // 39.6 deg fov for 50mm focal length

		// Create Materials
		float collectiveRoughness = 0.0f;
		m_materials["glass"] = new Glass(Vector3D(0.0f, 0.863f, 0.0f), collectiveRoughness, 1.45f);
		//m_materials["diffuse"] = new Lambertian(Vector3D(0.4f, 0.2f, 0.1f), 1.5f);
		m_materials["metal"] = new Metal(Vector3D(0.0f, 0.0f, 0.863f), collectiveRoughness, 1.45f);

		m_materials["ground"] = new Lambertian(Vector3D(0.5f, 0.5f, 0.5f), 1.45f);

		m_materials["dielectric1"] = new Dielectric(Vector3D(0.863f, 0.0f, 0.0f), collectiveRoughness, 1.45f);
		//m_materials["dielectric2"] = new Dielectric(Vector3D(0.863f, 0.0f, 0.0f), 0.5f, 1.45f);
		//m_materials["dielectric3"] = new Dielectric(Vector3D(0.863f, 0.0f, 0.0f), 1.0f, 1.45f);

		for (auto it = m_materials.begin(); it != m_materials.end(); it++) {
			(*it).second->CameraPos(lookFrom);
		}

		// Create Objects
		m_objects.push_back(new Sphere(Vector3D(0.0f, 1.0f, 2.5f), 1.0f, m_materials["dielectric1"]));
		m_objects.push_back(new Sphere(Vector3D(0.0f, 1.0f, 0.0f), 1.0f, m_materials["glass"]));
		m_objects.push_back(new Sphere(Vector3D(0.0f, 1.0f, -2.5f), 1.0f, m_materials["metal"]));
		m_objects.push_back(new Ground(0.0f, m_materials["ground"]));
	}

	size_t nextAvailable = 0;

	int maxXTiles = m_imageWidth / m_tileSize;
	int maxYTiles = m_imageHeight / m_tileSize;
	int maxTiles = maxXTiles * maxYTiles;

	int xTileSize = (int)roundf(m_imageWidth / (float)maxXTiles);
	int yTileSize = (int)roundf(m_imageHeight / (float)maxYTiles);

	int widthModulo = m_imageWidth % maxXTiles;
	int heightModulo = m_imageHeight % maxYTiles;

	int x = 0;
	while (x < m_imageWidth) {
		int addX = widthModulo > 0 ? xTileSize + 1 : xTileSize;

		int maxX = x + addX;
		maxX = maxX > m_imageWidth ? m_imageWidth : maxX;

		int l_heightModulo = heightModulo;

		int y = 0;
		while (y < m_imageHeight) {
			int addY = l_heightModulo > 0 ? yTileSize + 1 : yTileSize;

			int maxY = y + addY;
			maxY = maxY > m_imageHeight ? m_imageHeight : maxY;

			if (maxX > m_imageWidth || maxY > m_imageHeight) {
				y = y + 0;
			}

			m_tiles.push_back({ x, y, maxX, maxY });

			y = maxY;
			l_heightModulo--;
		}

		x = maxX;
		widthModulo--;
	}

	std::cout << "Total tiles: " << m_tiles.size() << '\n';
}

void Raytracing::SetRenderMode(const char* renderMode) {
	m_renderMode = renderMode;
}

bool Raytracing::Run() {
	if (m_renderMode == "normal") {
		m_renderNormals = true;
		m_renderAlbedo = false;
	}
	else if (m_renderMode == "albedo") {
		m_renderNormals = false;
		m_renderAlbedo = true;
	}
	else if (m_renderMode == "color" || m_renderMode == "colour") {
		m_renderNormals = false;
		m_renderAlbedo = false;
	}
	else {
		m_renderNormals = false;
		m_renderAlbedo = false;
	}

	// Render
	//size_t reserveThreads = (size_t)roundf(std::thread::hardware_concurrency() / 8.0f);
	//reserveThreads = reserveThreads > 0 ? reserveThreads : 1;
	const size_t maxThreads = std::thread::hardware_concurrency() /*- reserveThreads*/;
	//const size_t maxThreads = 0;

	//runTime.open("images/runTime.txt", std::ios_base::out);

	//m_log.open("log.txt", std::ios_base::out);
	if (m_renderAlbedo) {
		m_log.open("log_albedo.txt", std::ios_base::out);
	}
	else if (m_renderNormals) {
		m_log.open("log_normal.txt", std::ios_base::out);
	}
	else {
		m_log.open("log_color.txt", std::ios_base::out);
	}

	std::cout << "Threads Used: " << maxThreads << '\n';
	m_log << "Threads Used: " << maxThreads << '\n';

	/*std::vector<Tile> tiles;*/

	std::cout << "Total tiles: " << m_tiles.size() << '\n';
	m_log << "Total tiles: " << m_tiles.size() << '\n';

	//system("pause");

	m_tilesRendered = 0;

	size_t max = maxThreads < m_tiles.size() ? maxThreads : m_tiles.size();
	m_nextAvailable = 0;
	for (size_t i = 0; i < max; i++) {
		/*std::cout << "Rendering tile #" << m_nextAvailable << '\n';
		m_log << "Rendering tile #" << m_nextAvailable << '\n';*/

		m_threads.push_back(std::thread(&Raytracing::RenderTile, this, m_nextAvailable));
		m_threadId[m_threads[i].get_id()] = i + 1;
		m_nextAvailable++;
	}

	for (size_t i = 0; i < m_threads.size(); i++) {
		m_threads[i].join();
	}

	OrderedDithering(m_render, DitherFilter::FULLCOLOR, Threshold::ORDERED_8, 255);

	if (m_renderNormals) {
		m_render.Write("render_normal.png");
	}
	else if (m_renderAlbedo) {
		m_render.Write("render_albedo.png");
	}
	else {
		m_render.Write("render_color.png");
	}

	if (Image::PrintToConsole) system("pause");

	m_threads.clear();
	m_log.close();
	//m_tiles.clear();

	return true;
}

void Raytracing::RenderTile(const size_t startIndex) {
	auto start = std::chrono::high_resolution_clock::now();
	Render(m_tiles[startIndex].minX, m_tiles[startIndex].minY, m_tiles[startIndex].maxX, m_tiles[startIndex].maxY);
	auto end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::thread::id thisId = std::this_thread::get_id();

	//mtx.wa
	StaticMutex::s_mtx.lock();

	m_tilesRendered++;

	system("CLS");
	//float progress = (m_tilesRendered / (float)m_tiles.size()) * 100.0f;
	std::cout << "Render Mode: " << m_renderMode << '\n'
		<< "Threads Used: " << m_threads.size() << '\n'
		<< "Total Tiles: " << m_tiles.size() << '\n'
		<< "Progress: " << m_tilesRendered << "/" << m_tiles.size() << '\n';

	//std::cout << "Rendered tile #" << std::dec << startIndex << " in thread #" << std::dec << m_threadId[thisId] << std::dec << " for " << dur << '\n';
	m_log << "Rendered tile #" << std::dec << startIndex << " in thread #" << std::dec << m_threadId[thisId] << std::dec << " for " << dur << '\n';

	size_t nextAvailable = m_nextAvailable;
	m_nextAvailable++;

	StaticMutex::s_mtx.unlock();

	if (nextAvailable < m_tiles.size()) {
		RenderTile(nextAvailable);
	}
}

Vector3D Raytracing::BiLerp(const float x, const float y, Image& image) {
	int index = 0;

	index = image.GetIndex((int)floorf(x), (int)floorf(y));
	Vector3D Q11(image.GetDataF(index + 0) / 255.0f, image.GetDataF(index + 1) / 255.0f, image.GetDataF(index + 2) / 255.0f);

	index = image.GetIndex((int)floorf(x), (int)ceilf(y));
	Vector3D Q12(image.GetDataF(index + 0) / 255.0f, image.GetDataF(index + 1) / 255.0f, image.GetDataF(index + 2) / 255.0f);

	index = image.GetIndex((int)ceil(x), (int)floorf(y));
	Vector3D Q21(image.GetDataF(index + 0) / 255.0f, image.GetDataF(index + 1) / 255.0f, image.GetDataF(index + 2) / 255.0f);

	index = image.GetIndex((int)ceilf(x), (int)ceilf(y));
	Vector3D Q22(image.GetDataF(index + 0) / 255.0f, image.GetDataF(index + 1) / 255.0f, image.GetDataF(index + 2) / 255.0f);

	Vector3D R1 = Vector3D::Lerp(Q11, Q21, x - floorf(x));
	Vector3D R2 = Vector3D::Lerp(Q12, Q22, x - floorf(x));

	Vector3D p = Vector3D::Lerp(R1, R2, y - floorf(y));
	return p;
}

void Raytracing::UVSphere(Vector3D unitDir, float& u, float& v) {
	u = 0.5f + (atan2f(unitDir.GetX(), unitDir.GetZ()) / (2.0f * 3.14159265f));
	v = 0.5f - (asinf(unitDir.GetY()) / 3.14159265f);
}

Raytracing::~Raytracing() {
	// Destroy Pointers
	for (size_t i = 0; i < m_objects.size(); i++) {
		delete m_objects[i];
		m_objects[i] = nullptr;
	}
	m_objects.clear();

	for (auto it = m_materials.begin(); it != m_materials.end(); it++) {
		delete (*it).second;
		(*it).second = nullptr;
	}
	m_materials.clear();

	for (auto it = m_proceduralMats.begin(); it != m_proceduralMats.end(); it++) {
		delete (*it);
		*it = nullptr;
	}
	m_proceduralMats.clear();
}

const Vector3D Raytracing::RayColor(Ray& ray, const int depth) {
	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0) return Vector3D(0.0f, 0.0f, 0.0f);

	float clipStart = 0.0001f;
	float clipEnd = 1000.0f;

	unsigned int bitCount = 32;

	// draw objects
	HitRec rec;
	//rec.SetMaterial(m_materials["ground"]);
	if (HitObject(ray, clipStart, clipEnd, rec)) {
		Ray scattered;
		Vector3D attentuation;

		if (rec.GetMaterial() == nullptr || rec.GetMaterial()->Scatter(ray, rec, attentuation, scattered)) {
			if (m_renderNormals) {
				return (rec.GetNormal() + Vector3D(1.0f, 1.0f, 1.0f)) / 2.0f;
			}
			else if (m_renderAlbedo) {
				return attentuation;
			}
			else {
				Vector3D outColor;

				for (auto it = m_lights.begin(); it != m_lights.end(); it++) {
					// Shadow ray
					Vector3D shadowDir = (*it).GetPosition() - rec.GetPoint();
					float distSq = shadowDir.SqrMagnitude();
					shadowDir = shadowDir + Vector3D::RandomUnitVector(bitCount);
					shadowDir = shadowDir.UnitVector();

					Ray shadowRay = Ray(rec.GetPoint(), shadowDir);
					HitRec shadowRec;

					// Color
					Vector3D lightColor = (*it).GetColor();

					if (HitObject(shadowRay, clipStart, clipEnd, shadowRec)) {
						Vector3D shadowColor;

						if (shadowRec.GetMaterial()->IsTransparent()) {
							if (LinearFeedbackShift::RandFloat(bitCount) < shadowRec.GetMaterial()->GetRoughness()) {
								shadowColor = Vector3D(0.1f, 0.1f, 0.1f);
							}
							else {
								shadowColor = shadowRec.GetMaterial()->GetAlbedo();
							}
						}
						else {
							shadowColor = Vector3D(0.1f, 0.1f, 0.1f);
						}

						lightColor *= shadowColor;
					}

					outColor += (lightColor /** (*it).GetIntensity()*/) /*/ (4 * 3.14159265 * distSq)*/;
				}

				//outColor /= (float)m_lights.size();

				outColor = Vector3D(std::clamp(outColor.GetX(), 0.0f, 1.0f),
					std::clamp(outColor.GetY(), 0.0f, 1.0f),
					std::clamp(outColor.GetZ(), 0.0f, 1.0f));

				return attentuation * outColor * RayColor(scattered, depth - 1);
			}
		}

		//return rec.GetMaterial()->GetAlbedo();
		//return Vector3D(0.0f, 0.0f, 0.0f);
	}

	Vector3D unit_direction = ray.GetDirection().UnitVector();

	// draw backround
	if (m_renderNormals) {
		return (unit_direction + Vector3D(1.0f, 1.0f, 1.0f)) / 2.0f;
	}
	else {
		////unit_direction.UnitVector();
		//float t = 0.5f * (unit_direction.GetY() + 1.0f);
		float pi = 3.14159265f;

		//return (Vector3D(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vector3D(0.5f, 0.7f, 1.0f) * t);
		float u = 0.0f;
		float v = 0.0f;

		UVSphere(unit_direction, u, v);

		//int x = (int)roundf((m_hdri.GetWidth() - 1) * u);
		//int y = (int)roundf((m_hdri.GetHeight() - 1) * v);
		//int index = m_hdri.GetIndex(x, y);

		//float r = m_hdri.GetDataF(index + 0) / 255.0f;
		//float g = m_hdri.GetDataF(index + 1) / 255.0f;
		//float b = m_hdri.GetDataF(index + 2) / 255.0f;

		// bilinear texture interpolation
		float x = (m_hdri.GetWidth() - 1) * u;
		float y = (m_hdri.GetHeight() - 1) * v;

		return BiLerp(x, y, m_hdri);
	}
}

const bool Raytracing::HitObject(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	HitRec temp_rec;
	bool hit = false;
	float closest = t_max;

	for (auto it = m_objects.begin(); it != m_objects.end(); it++) {
		if ((*it)->Hit(ray, t_min, closest, temp_rec)) {
			hit = true;
			closest = temp_rec.GetT();
			rec = temp_rec;
		}
	}

	return hit;
}

void Raytracing::Render(const int minX, const int minY, const int maxX, const int maxY) {
	unsigned int bitCount = 32;

	for (int x = minX; x < maxX; x++) {
		for (int y = minY; y < maxY; y++) {
			int flippedY = (m_imageHeight - y) - 1;

			// ----- SET COLOR -----
			Vector3D pixel_color = Vector3D(0.0f, 0.0f, 0.0f);
			for (int s = 0; s < m_samplesPerPixel; s++) {
				float u = (x + LinearFeedbackShift::RandFloat(bitCount)) / (float)(m_imageWidth - 1);
				float v = (y + LinearFeedbackShift::RandFloat(bitCount)) / (float)(m_imageHeight - 1);

				Ray r = m_camera.GetRay(u, v);

				pixel_color += RayColor(r, m_maxDepth);
			}

			float scale = 1.0f / (float)m_samplesPerPixel;

			if (m_renderNormals || m_renderAlbedo) {
				// without gamma correction
				pixel_color *= scale;
			}
			else {
				// gamma correction
				pixel_color = Vector3D(sqrtf(pixel_color.GetX() * scale), sqrtf(pixel_color.GetY() * scale), sqrtf(pixel_color.GetZ() * scale));
			}

			pixel_color = Vector3D(std::clamp(pixel_color.GetX(), 0.0f, 1.0f), std::clamp(pixel_color.GetY(), 0.0f, 1.0f), std::clamp(pixel_color.GetZ(), 0.0f, 1.0f));
			pixel_color *= 255.0f;

			// ----- WRITE COLOR -----
			int index = m_render.GetIndex(x, flippedY);

			m_render.SetData(index + 0, pixel_color.GetX());
			m_render.SetData(index + 1, pixel_color.GetY());
			m_render.SetData(index + 2, pixel_color.GetZ());
		}
	}
}