#include "Raytracing.h"
#include "Filters.h"
#include "Sphere.h"

bool Image::PrintToConsole = false;

Raytracing::Raytracing() {
}

bool Raytracing::Run() {
	// Create Objects
	//Sphere sphere1(Vector3D(0, 0, -1), 0.5);
	m_objects.push_back(new Sphere(Vector3D(0.0f, 0.0f, -1.0f), 0.5f));
	m_objects.push_back(new Sphere(Vector3D(0.0f, -100.5f, -1.0f), 100.0f));

	// Image
	const float aspect_ratio = 16.0f / 9.0f;
	const int image_width = 1280;
	const int image_height = (int)(image_width / aspect_ratio);

	Image image(image_width, image_height, 3);

	// Camera
	float viewport_height = 2.0f;
	float viewport_width = aspect_ratio * viewport_height;
	float focal_length = 1.0f;

	Vector3D origin(0.0f, 0.0f, 0.0f);
	Vector3D horizontal(viewport_width, 0.0f, 0.0f);
	Vector3D vertical(0.0f, viewport_height, 0.0f);

	Vector3D lowerLeftCorner = origin;
	lowerLeftCorner -= (horizontal / 2.0f);
	lowerLeftCorner -= (vertical / 2.0f);
	lowerLeftCorner -= Vector3D(0.0f, 0.0f, focal_length);

	// Render
	for (int x = 0; x < image_width; x++) {
		for (int y = 0; y < image_height; y++) {
			int flippedY = (image_height - y) - 1;

			if (y == image_height - 10) {
				bool breakpoint = false;
			}

			if (x == image_width - 10) {
				bool breakpoint = false;
			}
			//int flippedY = y;

			float u = (float)x / (float)(image_width - 1);
			float v = (float)y / (float)(image_height - 1);

			Vector3D dir = lowerLeftCorner;
			Vector3D hU = horizontal;
			hU *= u;
			dir += hU;

			Vector3D vV = vertical;
			vV *= v;
			dir += vV;

			dir -= origin;

			Ray ray(origin, dir);
			Vector3D pixel_color = RayColor(ray);

			int index = image.GetIndex(x, flippedY);

			image.SetData(index + 0, pixel_color.GetX());
			image.SetData(index + 1, pixel_color.GetY());
			image.SetData(index + 2, pixel_color.GetZ());
		}
	}

	OrderedDithering(image, DitherFilter::FULLCOLOR, Threshold::ORDERED_8, 255);
	
	image.Write("images/render.png");

	// Destroy Pointers
	for (size_t i = 0; i < m_objects.size(); i++) {
		delete m_objects[i];
		m_objects[i] = nullptr;
	}
	m_objects.clear();

	return true;
}

Raytracing::~Raytracing() {
}

const Vector3D Raytracing::RayColor(Ray& ray) {
	// draw objects
	HitRec rec;
	if (HitObject(ray, 0.0f, INFINITY, rec)) {
		Vector3D col = rec.normal + Vector3D(1.0f, 1.0f, 1.0f);
		col *= 0.5f * 255.0f;

		return col;
	}

	// draw backround
	Vector3D unit_direction = ray.GetDirection();
	unit_direction.UnitVector();
	float t = 0.5f * (unit_direction.GetY() + 1.0f);

	return (Vector3D(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vector3D(0.5f, 0.7f, 1.0f) * t) * 255.0f;
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
