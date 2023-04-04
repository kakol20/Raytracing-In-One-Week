#include "../../misc/Random.h"
#include "../../wrapper/Fastwrite.h"

#include "OriginalScene.h"

OriginalScene::OriginalScene() {
}

OriginalScene::~OriginalScene() {
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

	m_settings = nullptr;
}

void OriginalScene::Create(Settings& settings) {
	m_settings = &settings;

	// ----- BACKGROUND -----

	m_bgStrength = 1;

	// ----- CAMERA -----

	int imageWidth = std::stoi(settings["imageWidth"]);
	int imageHeight = std::stoi(settings["imageHeight"]);

	Vector3D lookFrom(13, 2, 3);

	const Float aspectRatio = Float(imageWidth) / imageHeight;
	m_camera = Camera(aspectRatio, Float::FromString(settings["blurStrength"]), 10, Float::FromString(settings["verticalFOV"]), lookFrom, Vector3D::Zero, Vector3D::Up);

	m_clipEnd = Float::MaxVal;

	// ----- OBJECTS -----

	// materials

	m_matMap["frontSphere"] = new Metal(Vector3D(0.7, 0.6, 0.5), 0, 1.45);
	m_matMap["ground"] = new Diffuse(Vector3D(0.5, 0.5, 0.5));
	m_matMap["middleSphere"] = new Glass(Vector3D::One, 0, 1.5);
	m_matMap["rearSphere"] = new Diffuse(Vector3D(0.4, 0.2, 0.1));

	// objects

	m_renderedObjects.push_back(new Sphere(1, m_matMap["frontSphere"], Vector3D::Zero, Vector3D(4, 1, 0)));
	m_renderedObjects.push_back(new Sphere(1, m_matMap["middleSphere"], Vector3D::Zero, Vector3D(0, 1, 0)));
	m_renderedObjects.push_back(new Sphere(1, m_matMap["rearSphere"], Vector3D::Zero, Vector3D(-4, 1, 0)));

	for (int a = -10; a < 10; a++) {
		for (int b = -10; b < 10; b++) {
			std::string fastWriteOut = FastWrite::ResetString() + "Creating Scene:\na: " + std::to_string(a) + "\nb: " + std::to_string(b) + '\n';
			FastWrite::Write(fastWriteOut);

			Float chooseMat = Random::RandomFloat();
			Vector3D center(Random::RandomFloat(0, 0.9) + a, 0.2, Random::RandomFloat(0, 0.9) + b);

			bool intersect = false;
			for (auto it = m_renderedObjects.begin(); it != m_renderedObjects.end(); it++) {
				if ((*it)->SphereIntersectSphere(center, 0.2)) {
					intersect = true;
					break;
				}
			}

			if (!intersect) {
				std::string matID = "randomMat_" + std::to_string(m_renderedObjects.size());

				if (chooseMat < 0.9) {
					m_matMap[matID] = new Diffuse(Vector3D::RandomVector() * Vector3D::RandomVector());
				} else if (chooseMat < 0.95) {
					m_matMap[matID] = new Metal(Vector3D::RandomVector(0.5, 1), Random::RandomFloat(0, 0.5), 1.45);
				} else {
					m_matMap[matID] = new Glass(Vector3D::One, 0, 1.5);
				}

				m_renderedObjects.push_back(new Sphere(0.2, m_matMap[matID], Vector3D::One, center));
			}
		}
	}

	m_renderedObjects.push_back(new Sphere(1000, m_matMap["ground"], Vector3D::Zero, Vector3D(0, -1000, 0)));
}

Vector3D OriginalScene::BackgroundRay(const Vector3D& unitDir) {
	Float t = 0.5 * (unitDir.GetY() + 1);
	return ((Vector3D::One * (1 - t)) + (Vector3D(0.5, 0.7, 1) * t));
}