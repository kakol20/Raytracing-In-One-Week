#pragma once
#include "Material.h"
class Diffuse :
    public Material {
public:
	Diffuse();
	Diffuse(const Vector3D& albedo);
	virtual ~Diffuse();

	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);

public:
	virtual bool IsTransparent(Vector3D uv = Vector3D()) { return false; };
	virtual float GetRoughness(Vector3D uv = Vector3D()) { return m_roughness; };
	virtual Vector3D GetAlbedo(Vector3D uv = Vector3D()) { return m_albedo; };
};

