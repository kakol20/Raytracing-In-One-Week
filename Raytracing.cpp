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
	float PI = 3.14159265f;

	// Image
	m_imageWidth = 1280;
	m_imageHeight = 720;

	m_render = Image(m_imageWidth, m_imageHeight, 3);

	// Camera
	const float aspect_ratio = m_imageWidth / (float)m_imageHeight;
	Vector3D lookFrom(-2.0f, 2.0f,  1.0f);
	Vector3D lookAt(   0.0f, 0.0f, -1.0f);
	Vector3D up(       0.0f, 1.0f,  0.0f);
	m_camera = Camera(aspect_ratio, 39.6f, lookFrom, lookAt, up); // 39.6 deg fov for 50mm focal length

	float R = cosf(PI / 4.0f);

	// Create Materials
	m_materials["ground"] = new Lambertian(Vector3D(0.8f, 0.8f, 0.0f));
	m_materials["center"] = new Lambertian(Vector3D(0.1f, 0.2f, 0.5f));
	m_materials["left"] = new Glass(Vector3D(1.0f, 1.0f, 1.0f), 1.33f);
	m_materials["right"] = new Metal(Vector3D(0.8f, 0.6f, 0.2f), 0.0f);

	// Create Objects
	m_objects.push_back(new Sphere(Vector3D( 0.0f, -100.5f, -1.0f), 100.00f, m_materials["ground"]));
	m_objects.push_back(new Sphere(Vector3D( 0.0f,    0.0f, -1.0f),   0.50f, m_materials["center"]));
	m_objects.push_back(new Sphere(Vector3D(-1.0f,    0.0f, -1.0f),   0.50f, m_materials["left"]));
	m_objects.push_back(new Sphere(Vector3D(-1.0f,    0.0f, -1.0f),  -0.45f, m_materials["left"]));
	m_objects.push_back(new Sphere(Vector3D( 1.0f,    0.0f, -1.0f),   0.50f, m_materials["right"]));
		
	// Other
	m_samplesPerPixel = 128;
	m_maxDepth = 32;
	m_tileSize = 32;
}

bool Raytracing::Run() {
	// Render
	const size_t maxThreads = std::thread::hardware_concurrency();

	std::cout << "Max Threads: " << maxThreads << '\n';

	bool threaded = true;

	for (int x = 0; x < m_imageWidth; x += m_tileSize) {
		bool outOfBoundsX = x + m_tileSize <= m_imageWidth;

		for (int y = 0; y < m_imageHeight; y += m_tileSize) {
			bool outOfBoundsY = y + m_tileSize <= m_imageHeight;

			int maxX = outOfBoundsX ? x + m_tileSize : m_imageWidth;
			int maxY = outOfBoundsY ? y + m_tileSize : m_imageHeight;

			if (threaded) {
				m_threads.push_back(std::thread(&Raytracing::Render, this, x, y, maxX, maxY));

				if (m_threads.size() == maxThreads) {
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
		for (size_t i = 0; i < m_threads.size(); i++) {
			m_threads[i].join();
		}
	}

	OrderedDithering(m_render, DitherFilter::FULLCOLOR, Threshold::ORDERED_8, 255);

	m_render.Write("images/render.png");

	if (Image::PrintToConsole) system("pause");

	return true;
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