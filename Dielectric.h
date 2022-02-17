#pragma once
#include "Material.h"
class Dielectric :
    public Material {
public:
	Dielectric() : Dielectric(Vector3D(1.f, 1.f, 1.f), 0.f, 1.5f) {};
	Dielectric(const Vector3D& albedo, const float roughness, const float ior);
	virtual ~Dielectric() {};

	virtual bool Emission(HitRec& rec, Vector3D& emission) { return false; }
	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);

	virtual bool IsTransmissive(Vector3D uv = Vector3D()) { return false; };
	virtual float GetRoughness(Vector3D uv = Vector3D()) { return m_roughness; };
	virtual Vector3D GetAlbedo(Vector3D uv = Vector3D()) { return m_albedo; };
};

