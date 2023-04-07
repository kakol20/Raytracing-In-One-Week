#include "../../misc/ColorTools.h"
#include "../../misc/Random.h"

#include "FullScene.h"

FullScene::FullScene() {
}

FullScene::~FullScene() {
	if (!m_renderedObjects.empty()) {
		for (auto it = m_renderedObjects.begin(); it != m_renderedObjects.end(); it++) {
			delete (*it);
			(*it) = nullptr;
		}
		m_renderedObjects.clear();
	}

	if (!m_matMap.empty()) {
		for (auto it = m_matMap.begin(); it != m_matMap.end(); it++) {
			delete (*it).second;
			(*it).second = nullptr;
		}
		m_matMap.clear();
	}

	//if (!m_textures.empty()) {
	//	for (auto it = m_textures.begin(); it != m_textures.end(); it++) {
	//		delete (*it).second;
	//		(*it).second = nullptr;
	//	}
	//	m_textures.clear();
	//}

	m_settings = nullptr;
}

void FullScene::Create(Settings& settings) {
	//settings["blurStrength"] = "0";

	m_renderedObjects.reserve(600);

	m_settings = &settings;

	// ----- BACKGROUND -----

	m_background = Image("images/hdr/kiara_2_sunrise_4k.png", Image::Interpolation::Cubic, Image::Extrapolation::Repeat, Image::ColorSpace::sRGB);

	m_bgStrength = 0.1;

	// ----- CAMERA -----

	int imageWidth = std::stoi(settings["imageWidth"]);
	int imageHeight = std::stoi(settings["imageHeight"]);

	Vector3D lookFrom(13, 2, 3);

	const Float aspectRatio = Float(imageWidth) / imageHeight;
	m_camera = Camera(aspectRatio, Float::FromString(settings["blurStrength"]), 10, Float::FromString(settings["verticalFOV"]), lookFrom, Vector3D::Zero, Vector3D::Up);

	m_clipEnd = 63.27716808;

	// ----- OBJECTS -----

	// -- Materials --
	m_matMap["ground"] = new Diffuse(Vector3D(0.5, 0.5, 0.5));

	m_matMap["back"] = new Diffuse(Vector3D(0.4, 0.2, 0.1)); // TODO: replace with dielectric material
	m_matMap["middle"] = new Glass(Vector3D(1, 1, 1), 0, 1.5);
	m_matMap["front"] = new Metal(Vector3D(0.7, 0.6, 0.5), 0.04, 1.45);

	// -- Objects --

	// centre piece objects
	m_renderedObjects.push_back(new Sphere(1, m_matMap["back"], Vector3D::Zero, Vector3D(-4, 1, 0)));
	m_renderedObjects.push_back(new Sphere(1, m_matMap["middle"], Vector3D::Zero, Vector3D(0, 1, 0)));
	m_renderedObjects.push_back(new Sphere(1, m_matMap["front"], Vector3D::Zero, Vector3D(4, 1, 0)));

	// random objects

	unsigned int count = 0;
	for (int x = -10; x <= 10; x++) {
		for (int y = -10; y <= 10; y++) {
			Vector3D pos(x, 0.2, y);
			Vector3D randPos(Random::RandomFloatND() * 0.507107, 0, Random::RandomFloatND() * 0.507107);

			pos += randPos;

			bool intersect = false;

			for (auto it = m_renderedObjects.begin(); it != m_renderedObjects.end(); it++) {
				if ((*it)->SphereIntersectSphere(pos, 0.2)) {
					intersect = true;
					break;
				}
			}

			if (!intersect) {
				unsigned int objectType = (unsigned int)Random::RandomInt(0, 10);
				count++;

				if (objectType < 1) {
					// point light
					Float strength = Random::RandomFloat(1, 2);
					//m_renderedObjects.push_back(new PointLight(Unshaded(Vector3D::One * strength), 0.2, pos));

					// currently not working properly - using ushaded sphere as alternative
					std::string key = "randMat_" + std::to_string(count);
					m_matMap[key] = new Unshaded(ColorTools::KelvinToRGB(Random::RandomFloat(1000, 12200)) * strength);

					m_renderedObjects.push_back(new Sphere(0.2, m_matMap[key], Vector3D::Zero, pos));
				}
				else {
					// spheres
					unsigned int materialType = (unsigned int)Random::RandomInt(0, 3);

					Float hue = Random::RandomFloat(0, 360);

					std::string key = "randMat_" + std::to_string(count);

					if (materialType < 1) {
						Float s = (Float(240) - 12) / 204;
						Float v = 0.8;
						Float roughness = Random::RandomFloat();
						//m_matMap[key] = new Diffuse(ColorTools::HSVToRGB(hue, , 0.8));
						m_matMap[key] = new Dielectric(ColorTools::HSVToRGB(hue, s, v), roughness, 1.46);
					}
					else if (materialType < 2) {
						Float roughness = Random::RandomFloat(0, 1);
						Float s = Random::RandomFloat(0, 0.25);
						m_matMap[key] = new Glass(ColorTools::HSVToRGB(hue, s, 1), roughness, 1.5);
					}
					else {
						Float roughness = Random::RandomFloat(0, 1);
						Float s = Random::RandomFloat(0.25, 0.5);
						m_matMap[key] = new Metal(ColorTools::HSVToRGB(hue, s, 1), roughness, 1.45);
					}

					m_renderedObjects.push_back(new Sphere(0.2, m_matMap[key], Vector3D::Zero, pos));
				}
			}
		}
	}

	//ground
	m_renderedObjects.push_back(new Sphere(1000, m_matMap["ground"], Vector3D::Zero, Vector3D(0, -1000, 0)));

	// lights
	//m_renderedObjects.push_back(new PointLight(Unshaded(ColorTools::KelvinToRGB(5778) * 4), 8, Vector3D(20, 15, -10)));
	//m_renderedObjects.push_back(new PointLight(Unshaded(ColorTools::KelvinToRGB(5778) * 2), 16, Vector3D(20, 15, 10)));
	m_matMap["light1"] = new Unshaded(Unshaded(ColorTools::KelvinToRGB(5778) * 4));
	m_matMap["light2"] = new Unshaded(Unshaded(ColorTools::KelvinToRGB(5778) * 2));
	m_renderedObjects.push_back(new Sphere(4, m_matMap["light1"], Vector3D::Zero, Vector3D(20, 15, -10)));
	m_renderedObjects.push_back(new Sphere(8, m_matMap["light2"], Vector3D::Zero, Vector3D(20, 15, 10)));

	m_renderedObjects.shrink_to_fit();
}

Vector3D FullScene::BackgroundRay(const Vector3D& unitDir) {
	Vector3D uv = unitDir.UVSphere();
	uv *= Vector3D(-1, 1, 1);

	// for debugging purposes
	//uv *= 10;

	Float u = uv.GetX() * m_background.GetWidth();
	Float v = uv.GetY() * m_background.GetHeight();

	Float r, g, b;
	m_background.GetColor(u, v, r, g, b);

	Vector3D rgb(r, g, b);
	rgb /= 255;

	return rgb * m_bgStrength;
}