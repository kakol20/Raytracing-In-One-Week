#pragma once
#include "Material.h"
class Emissive :
    public Material {
public:
	Emissive();
	Emissive(const Vector3D& albedo, const float intensity);
	virtual ~Emissive();

	virtual bool Emission(HitRec& rec, Vector3D& emission);
	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);

public:
	virtual bool IsTransparent(Vector3D uv = Vector3D()) { return false; };
	virtual float GetRoughness(Vector3D uv = Vector3D()) { return 0.f; };
	virtual Vector3D GetAlbedo(Vector3D uv = Vector3D()) { return m_albedo; };

private:
	float m_intensity;
};

