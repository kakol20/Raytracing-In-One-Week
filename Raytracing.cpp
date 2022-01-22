#include <algorithm>
#include <ctime>
#include <iostream>

#include "Filters.h"
#include "Glass.h"
#include "Lambertian.h"
#include "LinearFeedbackShift.h"
#include "Metal.h"
#include "Raytracing.h"
#include "Sphere.h"

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

	m_renderMode = "color";

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

			//std::cout << line.GetFirst("=") << " = " << String::ToInt(line.GetSecond("=")) << "\n\n";
			String first = line.GetFirst("=");

			if (first == "aperture") {
				// float
				m_aperture = String::ToFloat(line.GetSecond("="));
			}
			else if (first == "imageHeight") {
				m_imageHeight = String::ToInt(line.GetSecond("="));
			}
			else if (first == "imageWidth") {
				m_imageWidth = String::ToInt(line.GetSecond("="));
			}
			else if (first == "maxDepth") {
				m_maxDepth = String::ToInt(line.GetSecond("="));
			}
			else if (first == "renderMode") {
				m_renderMode = line.GetSecond("=");

				if (m_renderMode == "normal") {
					m_renderNormals = true;
					m_renderAlbedo = false;
				}
				else if (m_renderMode == "albedo") {
					m_renderNormals = false;
					m_renderAlbedo = true;
				}
				else if (m_renderMode == "diffuse") {
					m_renderNormals = false;
					m_renderAlbedo = false;
				}
				else {
					m_renderNormals = false;
					m_renderAlbedo = false;
				}
			}
			else if (first == "randomSeed") {
				LinearFeedbackShift::Seed = (unsigned int)String::ToInt(line.GetSecond("="));
			}
			else if (first == "samplesPerPixel") {
				m_samplesPerPixel = String::ToInt(line.GetSecond("="));
			}
			else if (first == "tileSize") {
				m_tileSize = String::ToInt(line.GetSecond("="));
			}
			else if (first == "verticalFOV") {
				// float
				m_verticalFOV = String::ToFloat(line.GetSecond("="));
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
		settings << "maxDepth=" << m_maxDepth << '\n';
		settings << "#color, normal, albedo\n";
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

	// Camera
	const float aspect_ratio = m_imageWidth / (float)m_imageHeight;
	Vector3D lookFrom(13.0f, 2.0f, 3.0f);
	Vector3D lookAt(0.0f, 0.0f, 0.0f);
	Vector3D up(0.0f, 1.0f, 0.0f);
	Vector3D dist = Vector3D(4.0f, 1.0f, 0.0f) - lookFrom;
	m_camera = Camera(aspect_ratio, m_aperture, dist.Magnitude(), m_verticalFOV, lookFrom, lookAt, up); // 39.6 deg fov for 50mm focal length

	// Create Materials
	m_materials["glass"] = new Glass(Vector3D(1.0f, 1.0f, 1.0f), 0.0f, 1.5f);
	m_materials["diffuse"] = new Lambertian(Vector3D(0.4f, 0.2f, 0.1f));
	m_materials["metal"] = new Metal(Vector3D(0.7f, 0.6f, 0.5f), 0.0f, 1.5f);
	m_materials["ground"] = new Lambertian(Vector3D(0.5f, 0.5f, 0.5f));


	// Create Objects
	//m_objects.push_back(new Sphere(Vector3D(0.0f, 1.0f, 0.0f), -0.95f, m_materials["glass"]));
	m_objects.push_back(new Sphere(Vector3D(0.0f, 1.0f, 0.0f), 1.0f, m_materials["glass"]));
	m_objects.push_back(new Sphere(Vector3D(-4.0f, 1.0f, 0.0f), 1.0f, m_materials["diffuse"]));
	m_objects.push_back(new Sphere(Vector3D(4.0f, 1.0f, 0.0f), 1.0f, m_materials["metal"]));
	m_objects.push_back(new Sphere(Vector3D(0.0f, -1000.0f, 0.0f), 1000.0f, m_materials["ground"]));

	// Procedural Objects
	int index = 0;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float chooseMat = LinearFeedbackShift::RandFloat(32);
			Vector3D center((float)a + 0.9f * LinearFeedbackShift::RandFloat(32), 0.2f, (float)b + 0.9f * LinearFeedbackShift::RandFloat(32));

			Vector3D dist2 = center - Vector3D(4.0f, 2.0f, 0.0f);

			if (dist2.Magnitude() > 0.9f) {
				if (chooseMat < 0.8f) {
					// diffuse
					Vector3D albedo = Vector3D::Random(32) * Vector3D::Random(32);
					m_proceduralMats.push_back(new Lambertian(albedo));

					m_objects.push_back(new Sphere(center, 0.2f, m_proceduralMats[index]));
				}
				else if (chooseMat < 0.95f) {
					// metal
					Vector3D albedo = Vector3D::Random(0.5f, 1.0f, 32);
					float roughness = LinearFeedbackShift::RandFloat(32);
					m_proceduralMats.push_back(new Metal(albedo, roughness, 1.5f));

					m_objects.push_back(new Sphere(center, 0.2f, m_proceduralMats[index]));
				}
				else {
					m_proceduralMats.push_back(new Glass(Vector3D(1.0f, 1.0f, 1.0f), 1.33f, LinearFeedbackShift::RandFloatRange(0.0, 0.5, 32)));
					m_objects.push_back(new Sphere(center, 0.2f, m_proceduralMats[index]));
				}
				index++;
			}
		}
	}
}

bool Raytracing::Run() {
	// Render
	size_t reserveThreads = (size_t)roundf(std::thread::hardware_concurrency() / 8.0f);
	reserveThreads = reserveThreads > 0 ? reserveThreads : 1;
	const size_t maxThreads = std::thread::hardware_concurrency() - reserveThreads;
	//const size_t maxThreads = 0;

	//runTime.open("images/runTime.txt", std::ios_base::out);

	m_log.open("log.txt", std::ios_base::out);

	std::cout << "Threads Used: " << maxThreads << '\n';
	m_log << "Threads Used: " << maxThreads << '\n';

	/*std::vector<Tile> tiles;*/
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
	m_log << "Total tiles: " << m_tiles.size() << '\n';

	system("pause");

	size_t max = maxThreads < m_tiles.size() ? maxThreads : m_tiles.size();
	for (size_t i = 0; i < max; i++) {
		/*std::cout << "Rendering tile #" << m_nextAvailable << '\n';
		m_log << "Rendering tile #" << m_nextAvailable << '\n';*/

		m_threads.push_back(std::thread(&Raytracing::RenderTile, this, m_nextAvailable));
		m_threadId[m_threads[i].get_id()] = i + 1;
;		m_nextAvailable++;
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

	return true;
}

void Raytracing::RenderTile(const size_t startIndex) {

	auto start = std::chrono::high_resolution_clock::now();
	Render(m_tiles[startIndex].minX, m_tiles[startIndex].minY, m_tiles[startIndex].maxX, m_tiles[startIndex].maxY);
	auto end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::thread::id thisId = std::this_thread::get_id();

	std::mutex mtx;
	mtx.lock();

	std::cout << "Rendered tile #" << std::dec << startIndex << " in thread #" << std::dec << m_threadId[thisId] << std::dec << " for " << dur << '\n';
	m_log << "Rendered tile #" << std::dec << startIndex << " in thread #" << std::dec << m_threadId[thisId] << std::dec << " for " << dur << '\n';

	size_t nextAvailable = m_nextAvailable;
	m_nextAvailable++;

	mtx.unlock();

	if (nextAvailable < m_tiles.size()) {
		RenderTile(nextAvailable);
	}
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

	// draw objects
	HitRec rec;
	//rec.SetMaterial(m_materials["ground"]);
	if (HitObject(ray, 0.001f, INFINITY, rec)) {
		Ray scattered;
		Vector3D attentuation;

		if (rec.GetMaterial() == nullptr || rec.GetMaterial()->Scatter(ray, rec, attentuation, scattered)) {
			if (m_renderNormals) {
				return rec.GetNormal();
			}
			else if (m_renderAlbedo) {
				return attentuation;
			}
			else {
				return attentuation * RayColor(scattered, depth - 1);
			}
		}

		return Vector3D(0.0f, 0.0f, 0.0f);
	}

	// draw backround
	if (m_renderNormals) {
		return Vector3D(0.0f, 0.0f, 0.0f);
	}
	else {
		Vector3D unit_direction = ray.GetDirection().UnitVector();
		//unit_direction.UnitVector();
		float t = 0.5f * (unit_direction.GetY() + 1.0f);

		return (Vector3D(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vector3D(0.5f, 0.7f, 1.0f) * t)/* * 255.0f*/;
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
	for (int x = minX; x < maxX; x++) {
		for (int y = minY; y < maxY; y++) {
			int flippedY = (m_imageHeight - y) - 1;

			// ----- SET COLOR -----
			Vector3D pixel_color = Vector3D(0.0f, 0.0f, 0.0f);
			for (int s = 0; s < m_samplesPerPixel; s++) {
				float u = (x + LinearFeedbackShift::RandFloat(32)) / (float)(m_imageWidth - 1);
				float v = (y + LinearFeedbackShift::RandFloat(32)) / (float)(m_imageHeight - 1);

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