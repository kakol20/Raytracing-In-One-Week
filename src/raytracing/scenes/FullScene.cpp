#include "../../misc/ColorTools.h"

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

	//ground
	m_renderedObjects.push_back(new Sphere(1000, m_matMap["ground"], Vector3D::Zero, Vector3D(0, -1000, 0)));

	// lights
	m_renderedObjects.push_back(new PointLight(Unshaded(ColorTools::KelvinToRGB(5778) * 4), 8, Vector3D(20, 15, -10)));
	m_renderedObjects.push_back(new PointLight(Unshaded(ColorTools::KelvinToRGB(5778) * 2), 16, Vector3D(20, 15, 10)));

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
