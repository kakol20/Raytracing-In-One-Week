#include "DebugScene.h"

DebugScene::DebugScene() {
}

DebugScene::~DebugScene() {
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

void DebugScene::Create(Settings& settings) {
	// ----- BACKGROUND -----
	m_background = Image("images/hdr/lebombo_2k.png", Image::Interpolation::Cubic, Image::Extrapolation::Repeat, Image::ColorSpace::sRGB);

	m_bgStrength = 0.1;

	// ----- CAMERA -----

	int imageWidth = std::stoi(settings["imageWidth"]);
	int imageHeight = std::stoi(settings["imageHeight"]);

	m_clipStart = Float::NearZero;
	m_clipEnd = 1000;

	Vector3D lookFrom(0, 2, 20);
	Vector3D lookAt(0, 1, 0);

	Vector3D dist = lookAt - lookFrom;

	settings["blurStrength"] = "0";

	const Float aspectRatio = Float(imageWidth) / imageHeight;
	m_camera = Camera(aspectRatio, 0, dist.Magnitude(), Float::FromString(settings["verticalFOV"]), lookFrom, lookAt, Vector3D::Up);

	// ----- OBJECTS -----
	std::vector<Vector3D> colors;
	colors.push_back(Vector3D(255, 1, 1));
	colors.push_back(Vector3D(204, 255, 1));
	colors.push_back(Vector3D(1, 255, 102));
	colors.push_back(Vector3D(1, 102, 255));
	colors.push_back(Vector3D(204, 1, 255));

	for (auto it = colors.begin(); it != colors.end(); it++) {
		Vector3D& i = (*it);
		i /= 255;
	}

	m_matMap["diffuse"] = new Diffuse(colors[0]);
	m_matMap["unshaded"] = new Unshaded(colors[1] * 2);
	m_matMap["metallic"] = new Metal(colors[2], 0.1, 1.45);
	m_matMap["glass"] = new Glass(colors[3], 0.1, 1.45);

	m_matMap["ground"] = new Diffuse(Vector3D(0.5, 0.5, 0.5));

	// objects

	m_renderedObjects.push_back(new Sphere(1, m_matMap["diffuse"], Vector3D::Zero, Vector3D(-4.2, 1, 0)));
	m_renderedObjects.push_back(new Sphere(1, m_matMap["unshaded"], Vector3D::Zero, Vector3D(-2.1, 1, 0)));
	m_renderedObjects.push_back(new Sphere(1, m_matMap["glass"], Vector3D::Zero, Vector3D(0, 1, 0)));
	m_renderedObjects.push_back(new Sphere(1, m_matMap["metallic"], Vector3D::Zero, Vector3D(2.1, 1, 0)));
	m_renderedObjects.push_back(new PointLight(Unshaded(colors[4] * 3), 1, Vector3D(4.2, 1, 0)));

	m_renderedObjects.push_back(new Sphere(1000, m_matMap["ground"], Vector3D::Zero, Vector3D(0, -1000, 0)));

	m_settings = &settings;
}

Vector3D DebugScene::BackgroundRay(const Vector3D& unitDir) {
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
