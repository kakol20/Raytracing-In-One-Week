#pragma once
#include <vector>

#include "Plane.h"

#include "Object.h"
class Box :
    public Object {
public:
    Box() : Box(Vector3D(1.f), nullptr) {};
    Box(Vector3D size, Material* mat, const Vector3D uvScale = 1.f);
    virtual ~Box();

    virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

    virtual bool SphereIntersectGround(const Vector3D pos, const float radius) { return false; };
    virtual bool SphereIntersectSphere(const Vector3D pos, const float radius) { return false; };

private:
    std::vector<Plane*> m_planes;
};

