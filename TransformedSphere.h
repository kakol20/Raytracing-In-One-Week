#pragma once
#include <vector>

#include "Quaternion.h"

//#define MULTI_ROTATION

#include "Object.h"
class TransformedSphere :
    public Object {
public:
    TransformedSphere() : TransformedSphere(1.f, nullptr, Vector3D(), Vector3D()) {};

    /// <summary>
    /// 
    /// </summary>
    /// <param name="radius"></param>
    /// <param name="mat"></param>
    /// <param name="rotation">in degrees</param>
    /// <param name="translation"></param>
    /// <param name="uvScale"></param>
    TransformedSphere(const float& radius, Material* mat, const Vector3D& rotation, const Vector3D& translation, bool flipNormals = false, const Vector3D uvScale = Vector3D(1.f));

    ~TransformedSphere();

    virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

    virtual bool SphereIntersectGround(const Vector3D pos, const float radius) { return false; };
    virtual bool SphereIntersectSphere(const Vector3D pos, const float radius);

private:
#ifdef MULTI_ROTATION
    /// <summary>
    /// In XYZ Rotation order
    /// </summary>
    std::vector<Quaternion> m_rotation;
#else
    Quaternion m_rotation;
    Quaternion m_rotationInv;
#endif // TRANSFORMED_SPHERE_REPEAT

    bool m_flipNormals;
    float m_radius;

    bool TranslationHit(Ray& ray, const float t_min, const float t_max, HitRec& rec);
    bool RotationHit(Ray& ray, const float t_min, const float t_max, HitRec& rec);
    bool LocalHit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

    Vector3D CalculateTangent(HitRec& rec);
    Vector3D CalculateUV(const Vector3D point);
};

