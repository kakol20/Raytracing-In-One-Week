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
	m_imageWidth = 1280;
	m_imageHeight = 720;

	// Other
	m_samplesPerPixel = 128;
	m_maxDepth = 12;
	m_tileSize = 32;

	m_nextAvailable = 0;
}

void Raytracing::Init() {
	float PI = 3.14159265f;

	// Image
	m_render = Image(m_imageWidth, m_imageHeight, 3);

	// Camera
	const float aspect_ratio = m_imageWidth / (float)m_imageHeight;
	Vector3D lookFrom(13.0f, 2.0f, 3.0f);
	Vector3D lookAt(0.0f, 0.0f, 0.0f);
	Vector3D up(0.0f, 1.0f, 0.0f);
	Vector3D dist = lookAt - lookFrom;
	m_camera = Camera(aspect_ratio, 0.1f, dist.Magnitude(), 20.0f, lookFrom, lookAt, up); // 39.6 deg fov for 50mm focal length

	m_materials["ground"] = new Lambertian(Vector3D(0.5f, 0.5f, 0.5f));
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
					Vector3D albedo = Vector3D::Random() * Vector3D::Random();
					//albedo = albedo.UnitVector() * LinearFeedbackShift::RandFloatRange(0.5f, 0.75f, 32);
					m_proceduralMats.push_back(new Lambertian(albedo));

					m_objects.push_back(new Sphere(center, 0.2f, m_proceduralMats[index]));
				}
				else if (chooseMat < 0.95f) {
					// metal
					Vector3D albedo = Vector3D::Random(0.5f, 1.0f);
					float roughness = LinearFeedbackShift::RandFloat(32);
					m_proceduralMats.push_back(new Metal(albedo, roughness));

					m_objects.push_back(new Sphere(center, 0.2f, m_proceduralMats[index]));
				}
				else {
					m_proceduralMats.push_back(new Glass(Vector3D(1.0f, 1.0f, 1.0f), 1.33f));
					m_objects.push_back(new Sphere(center, 0.2f, m_proceduralMats[index]));
				}
				index++;
			}
		}
	}

	// Create Materials
	m_materials["glass"] = new Glass(Vector3D(1.0f, 1.0f, 1.0f), 1.33f);
	m_materials["diffuse"] = new Lambertian(Vector3D(0.4f, 0.2f, 0.1f));
	m_materials["metal"] = new Metal(Vector3D(0.7f, 0.6f, 0.5f), 0.0f);

	// Create Objects
	m_objects.push_back(new Sphere(Vector3D(0.0f, 1.0f, 0.0f), 1.0f, m_materials["glass"]));
	m_objects.push_back(new Sphere(Vector3D(-4.0f, 1.0f, 0.0f), 1.0f, m_materials["diffuse"]));
	m_objects.push_back(new Sphere(Vector3D(4.0f, 1.0f, 0.0f), 1.0f, m_materials["metal"]));
}

bool Raytracing::Run() {
	// Render
	const size_t maxThreads = std::thread::hardware_concurrency() - 1;
	//const size_t maxThreads = 0;

	std::cout << "Max Threads: " << maxThreads << '\n';

	bool threaded = true;

	/*
	for (int x = 0; x < m_imageWidth; x += m_tileSize) {
		bool outOfBoundsX = x + m_tileSize <= m_imageWidth;

		for (int y = 0; y < m_imageHeight; y += m_tileSize) {
			bool outOfBoundsY = y + m_tileSize <= m_imageHeight;

			int maxX = outOfBoundsX ? x + m_tileSize : m_imageWidth;
			int maxY = outOfBoundsY ? y + m_tileSize : m_imageHeight;

			if (threaded) {
				m_threads.push_back(std::thread(&Raytracing::Render, this, x, y, maxX, maxY));

				if (maxThreads > 0 && m_threads.size() == maxThreads) {
					std::cout << m_threads.size() << " threads used\n";

					for (size_t i = 0; i < m_threads.size(); i++) {
						m_threads[i].join();
					}
					m_threads.clear();
				}
			}
			else {
				Render(x, y, x + m_tileSize, y + m_tileSize);
			}
		}
	}

	if (threaded) {
		std::cout << m_threads.size() << " threads used\n";
		for (size_t i = 0; i < m_threads.size(); i++) {
			m_threads[i].join();
		}
	}
	*/

	/*std::vector<Tile> tiles;*/
	size_t nextAvailable = 0;

	for (int x = 0; x < m_imageWidth; x += m_tileSize) {
		bool outOfBoundsX = x + m_tileSize <= m_imageWidth;

		for (int y = 0; y < m_imageHeight; y += m_tileSize) {
			bool outOfBoundsY = y + m_tileSize <= m_imageHeight;

			int maxX = outOfBoundsX ? x + m_tileSize : m_imageWidth;
			int maxY = outOfBoundsY ? y + m_tileSize : m_imageHeight;

			m_tiles.push_back({ x, y, maxX, maxY });
		}
	}

	std::cout << "Total tiles: " << m_tiles.size() << '\n';

	int max = maxThreads < m_tiles.size() ? maxThreads : m_tiles.size();
	for (int i = 0; i < max; i++) {
		std::cout << "Rendering tile #" << m_nextAvailable << '\n';
		m_threads.push_back(std::thread(&Raytracing::RenderTile, this, m_nextAvailable));
		m_nextAvailable++;
	}

	for (size_t i = 0; i < m_threads.size(); i++) {
		m_threads[i].join();
	}

	OrderedDithering(m_render, DitherFilter::FULLCOLOR, Threshold::ORDERED_8, 255);

	m_render.Write("images/render.png");

	if (Image::PrintToConsole) system("pause");

	return true;
}

void Raytracing::RenderTile(const size_t startIndex) {
	Render(m_tiles[startIndex].minX, m_tiles[startIndex].minY, m_tiles[startIndex].maxX, m_tiles[startIndex].maxY);

	std::mutex mtx;
	mtx.lock();
	if (m_nextAvailable < m_tiles.size()) 		{
		std::cout << "Rendering tile #" << m_nextAvailable << '\n';
	}
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
			return attentuation * RayColor(scattered, depth - 1);
		}

		return Vector3D(0.0f, 0.0f, 0.0f);
	}

	// draw backround
	Vector3D unit_direction = ray.GetDirection().UnitVector();
	//unit_direction.UnitVector();
	float t = 0.5f * (unit_direction.GetY() + 1.0f);

	return (Vector3D(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vector3D(0.5f, 0.7f, 1.0f) * t)/* * 255.0f*/;
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

			pixel_color = Vector3D(sqrtf(pixel_color.GetX() * scale), sqrtf(pixel_color.GetY() * scale), sqrtf(pixel_color.GetZ() * scale)); // gamma correction

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
