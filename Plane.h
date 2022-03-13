#pragma once
#include "Quaternion.h"

#include "Object.h"
class Plane :
    public Object {
public:
    enum class Type {
        XPlus, XMinus, YPlus, YMinus, ZPlus, ZMinus
    };
public:
    Plane() : Plane(Plane::Type::YPlus, Vector3D(), 0.f, 0.f, nullptr) {};
    Plane(Plane::Type type, const Vector3D origin, const float width, const float height, Material* mat, const Vector3D uvScale = 1.f);
    virtual ~Plane();

    virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

    virtual bool SphereIntersectGround(const Vector3D pos, const float radius) { return false; };
    virtual bool SphereIntersectSphere(const Vector3D pos, const float radius) { return false; };

private:
    float m_height;
    float m_width;
    Quaternion m_rotation;
    Plane::Type m_type;
};

