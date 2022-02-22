#pragma once
#include "Material.h"
class Diffuse :
	public Material {
public:
	Diffuse();
	Diffuse(const Vector3D& albedo);
	virtual ~Diffuse();

	virtual bool Emission(HitRec& rec, Vector3D& emission);
	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);
	virtual Vector3D GetNormal(HitRec& rec) { return rec.GetNormal(); };

public:
	virtual bool IsTransmissive(Vector3D uv = Vector3D()) { return false; };
	virtual float GetRoughness(Vector3D uv = Vector3D()) { return m_roughness; };
	virtual Vector3D GetAlbedo(Vector3D uv = Vector3D()) { return m_albedo; };
};

