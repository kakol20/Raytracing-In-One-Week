#pragma once
#include "Material.h"
class Glass :
    public Material {
public:
	Glass() : Glass(Vector3D(1.f, 1.f, 1.f), 0.f, 1.5f) {};
	Glass(const Vector3D& albedo, const float roughness, const float ior);
	virtual ~Glass() {};

	virtual bool Emission(HitRec& rec, Vector3D& emission);
	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);
	virtual Vector3D GetNormal(HitRec& rec) { return rec.GetNormal(); };

	virtual bool IsTransmissive(Vector3D uv = Vector3D()) { return true; };
	virtual float GetRoughness(Vector3D uv = Vector3D()) { return m_roughness; };
	virtual Vector3D GetAlbedo(Vector3D uv = Vector3D()) { return m_albedo; };
};

