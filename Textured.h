#pragma once
#include "Image.h"

#include "Material.h"
class Textured :
	public Material {
public:
	Textured() : Textured(nullptr, nullptr, nullptr, 1.45f, Vector3D()) {};
	Textured(Image* albedo, Image* roughnessMetalEmission, Image* normal, const float ior, const Vector3D offset = Vector3D());
	virtual ~Textured();

	virtual bool Emission(HitRec& rec, Vector3D& emission);
	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);

	virtual bool IsTransmissive(Vector3D uv = Vector3D()) { return false; };
	virtual float GetRoughness(Vector3D uv = Vector3D());
	virtual Vector3D GetAlbedo(Vector3D uv = Vector3D());

private:
	Image* m_albedoTexture;
	Image* m_rmeTexture;
	Image* m_normalTexture;
	Vector3D m_uvOffset;
};
