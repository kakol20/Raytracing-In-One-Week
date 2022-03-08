#include "RotatedObj.h"

RotatedObj::RotatedObj(const Vector3D degrees, Object* object) {
	m_radians = degrees * Vector3D(3.14159265f / 180.f);
	//m_radians *= Vector3D(1.f, 1.f, 1.f);
	m_object = object;
}

RotatedObj::~RotatedObj() {
	m_object = nullptr;
}

bool RotatedObj::Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec) {
	if (m_object == nullptr) return false;

	Vector3D xUp = Vector3D(1.f, 0.f, 0.f);
	Vector3D yUp = Vector3D(0.f, 1.f, 0.f);
	Vector3D zUp = Vector3D(0.f, 0.f, 1.f);

	// X axis rotation
	Vector3D xRayOrig = Vector3D::RotateAxis(ray.GetOrig(), xUp, -m_radians.GetX());
	Vector3D xRayDir = Vector3D::RotateAxis(ray.GetDir(), xUp, -m_radians.GetX());
	//Ray xRay(xRayOrig, xRayDir);

	// Y axis rotation
	Vector3D yRayOrig = Vector3D::RotateAxis(xRayOrig, yUp, -m_radians.GetY());
	Vector3D yRayDir = Vector3D::RotateAxis(xRayDir, yUp, -m_radians.GetY());
	//Ray yRay(yRayOrig, yRayDir);

	// Z axis rotation
	Vector3D zRayOrig = Vector3D::RotateAxis(yRayOrig, zUp, -m_radians.GetZ());
	Vector3D zRayDir = Vector3D::RotateAxis(yRayDir, zUp, -m_radians.GetZ());
	Ray zRay(zRayOrig, zRayDir);

	if (!m_object->Hit(zRay, t_min, t_max, rec)) return false;

	Vector3D zPoint = Vector3D::RotateAxis(rec.GetPoint(), zUp, m_radians.GetZ());
	Vector3D zNormal = Vector3D::RotateAxis(rec.GetNormal(), zUp, m_radians.GetZ());

	Vector3D yPoint = Vector3D::RotateAxis(zPoint, yUp, m_radians.GetY());
	Vector3D yNormal = Vector3D::RotateAxis(zNormal, yUp, m_radians.GetY());

	Vector3D xPoint = Vector3D::RotateAxis(yPoint, xUp, m_radians.GetX());
	Vector3D xNormal = Vector3D::RotateAxis(yNormal, xUp, m_radians.GetX());

	rec.SetPoint(xPoint);
	rec.SetNormal(xNormal);

	return true;
}

bool RotatedObj::SphereIntersectGround(const Vector3D pos, const float radius) {
	return m_object->SphereIntersectGround(pos, radius);
}

bool RotatedObj::SphereIntersectSphere(const Vector3D pos, const float radius) {
	return m_object->SphereIntersectSphere(pos, radius);
}