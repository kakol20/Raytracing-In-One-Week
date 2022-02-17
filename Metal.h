#pragma once
#include "Material.h"
class Metal :
	public Material {
public:
	Metal() : Metal(Vector3D(1.f, 1.f, 1.f), 0.f, 1.5f) {};
	Metal(const Vector3D& albedo, const float roughness, const float ior);
	virtual ~Metal() {};

	virtual bool Emission(HitRec& rec, Vector3D& emission);
	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);

	virtual bool IsTransmissive(Vector3D uv = Vector3D()) { return false; };
	virtual float GetRoughness(Vector3D uv = Vector3D()) { return m_roughness; };
	virtual Vector3D GetAlbedo(Vector3D uv = Vector3D()) { return m_albedo; };

private:
	Vector3D m_edgeTint;
};

