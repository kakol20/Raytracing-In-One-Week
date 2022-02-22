#pragma once
#include "Image.h"

#include "Material.h"
class Textured :
	public Material {
public:
	Textured() : Textured(nullptr, nullptr, nullptr, 1.45f) {};
	Textured(Image* albedo, Image* roughnessMetalEmission, Image* normal, const float ior, const Vector3D offset = Vector3D(), const float emissionStrength = 1.f);
	virtual ~Textured();

	virtual bool Emission(HitRec& rec, Vector3D& emission);
	virtual bool Scatter(Ray& rayIn, HitRec& rec, Vector3D& attentuation, Ray& scattered);
	virtual Vector3D GetNormal(HitRec& rec);

	virtual bool IsTransmissive(Vector3D uv = Vector3D()) { return false; };
	virtual float GetRoughness(Vector3D uv = Vector3D());
	virtual Vector3D GetAlbedo(Vector3D uv = Vector3D());

private:
	float m_emissionStrength;
	Image* m_albedoTexture;
	Image* m_normalTexture;
	Image* m_rmeTexture;
	Vector3D m_uvOffset;
};
