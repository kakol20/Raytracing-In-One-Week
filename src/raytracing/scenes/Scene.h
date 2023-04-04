#pragma once

#include <map>

#include "../../misc/BlueNoise.h"
#include "../../wrapper/HDR.h"
#include "../../wrapper/Image.h"
#include "../Camera.h"
#include "../materials/Diffuse.h"
#include "../materials/Glass.h"
#include "../materials/Material.h"
#include "../materials/Metal.h"
#include "../materials/Unshaded.h"
#include "../objects/Object.h"
#include "../objects/PointLight.h"
#include "../objects/Sphere.h"
#include "../Settings.h"

class Scene {
public:
	Scene() {
		m_settings = nullptr;
	};
	virtual ~Scene() {};

	virtual void Create(Settings& settings) = 0;
	Vector3D RayColor(Ray& ray, const int depth, const Vector3D& initialRayCol = Vector3D::One);


	size_t ObjectCount() const { return m_renderedObjects.size(); };
	Camera GetCamera() const { return m_camera; };

private:
	bool RayHitObject(Ray& ray, const Float t_min, const Float t_max, HitRec& rec);

	virtual Vector3D BackgroundRay(const Vector3D& unitDir) = 0;

protected:
	Camera m_camera;
	Float m_clipEnd;
	Float m_clipStart;
	Image m_background;
	Settings* m_settings;
	Float m_bgStrength;

	// ----- OBJECTS -----

	std::map<std::string, Material*> m_matMap;
	std::vector<Object*> m_renderedObjects;
};