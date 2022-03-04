#include "Box.h"

Box::Box(Vector3D size, Material* mat, const Vector3D uvScale) {
	m_planes.reserve(6);

	m_mat = mat;
	m_uvScale = uvScale;
	m_pos = Vector3D(0.f);

	Vector3D axisPos = size / 2.f;

	// X Facing Planes
	m_planes.push_back(new Plane(Plane::Type::XMinus, Vector3D(-axisPos.GetX(), 0.f, 0.f), size.GetZ(), size.GetY(), mat, uvScale));
	m_planes.push_back(new Plane(Plane::Type::XPlus, Vector3D(axisPos.GetX(), 0.f, 0.f), size.GetZ(), size.GetY(), mat, uvScale));

	// Y Facing Planes
	m_planes.push_back(new Plane(Plane::Type::YMinus, Vector3D(0.f, -axisPos.GetY(), 0.f), size.GetX(), size.GetZ(), mat, uvScale));
	m_planes.push_back(new Plane(Plane::Type::YPlus, Vector3D(0.f, axisPos.GetY(), 0.f), size.GetX(), size.GetZ(), mat, uvScale));

	// Z Facing Planes
	m_planes.push_back(new Plane(Plane::Type::ZMinus, Vector3D(0.f, 0.f, -axisPos.GetZ()), size.GetX(), size.GetY(), mat, uvScale));
	m_planes.push_back(new Plane(Plane::Type::ZPlus, Vector3D(0.f, 0.f, axisPos.GetZ()), size.GetX(), size.GetY(), mat, uvScale));
}

Box::~Box() {
	if (!m_planes.empty()) {
		for (auto it = m_planes.begin(); it != m_planes.end(); it++) {
			delete (*it);
			(*it) = nullptr;
		}
		m_planes.clear();
	}

	m_mat = nullptr;
}

bool Box::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	HitRec tempRec;
	bool hit = false;
	float closest = t_max;

	for (auto it = m_planes.begin(); it != m_planes.end(); it++) {
		if ((*it)->Hit(ray, t_min, closest, tempRec)) {
			hit = true;
			closest = tempRec.GetT();
			rec = tempRec;
		}
	}
	return hit;
}
