#include <algorithm>
#include <ctime>
#include <iostream>

#include "Raytracing.h"
#include "Filters.h"
#include "Sphere.h"
#include "LinearFeedbackShift.h"

bool Image::PrintToConsole = false;

Raytracing::Raytracing() {

	// Create Objects
	//Sphere sphere1(Vector3D(0, 0, -1), 0.5);
	m_objects.push_back(new Sphere(Vector3D(0.0f, 0.0f, -1.0f), 0.5f));
	m_objects.push_back(new Sphere(Vector3D(0.0f, -600.5f, -1.0f), 600.0f));

	// Image
	m_imageWidth = 1280;
	m_imageHeight = 720;
	m_samplesPerPixel = 32;

	m_render = Image(m_imageWidth, m_imageHeight, 3);

	// Camera
	const float aspect_ratio = m_imageWidth / (float)m_imageHeight;
	float viewport_height = 2.0f;
	float viewport_width = aspect_ratio * viewport_height;
	float focal_length = 1.0f;

	m_camera = Camera(aspect_ratio, viewport_height, viewport_width, focal_length);

	// Other
	m_maxDepth = 8;
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
}

const Vector3D Raytracing::RandomInUnitSphere() {
	while (true) {
		Vector3D p = Vector3D::Random(-1.0f, 1.0f);

		if (p.SqrMagnitude() >= 1) continue;

		return p;
	}

	return Vector3D();
}

const Vector3D Raytracing::RayColor(Ray& ray, const int depth) {
	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0) return Vector3D(0.0f, 0.0f, 0.0f);

	// draw objects
	HitRec rec;
	if (HitObject(ray, 0.001f, INFINITY, rec)) {
		//Vector3D col = rec.normal + Vector3D(1.0f, 1.0f, 1.0f);
		//col *= 0.5f/* * 255.0f*/;

		Vector3D target = rec.point + rec.normal + RandomInUnitSphere();
		Ray tempRay = Ray(rec.point, target - rec.point);

		return RayColor(tempRay, depth - 1) * 0.5f;
	}

	// draw backround
	Vector3D unit_direction = ray.GetDirection();
	unit_direction.UnitVector();
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
			closest = temp_rec.t;
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

			float scale = 1.0f / m_samplesPerPixel;

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