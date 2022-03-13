#pragma once
#include "Quaternion.h"
#include "Object.h"
class TransformedObject :
	public Object {
public:
	TransformedObject() : TransformedObject(Vector3D(1.f), Vector3D(0.f), Vector3D(0.f), nullptr) {};

	/// <summary>
	/// Full transformation
	/// </summary>
	/// <param name="scale"></param>
	/// <param name="rotation">in degrees</param>
	/// <param name="translation"></param>
	/// <param name="object"></param>
	TransformedObject(const Vector3D scale, const Vector3D rotation, const Vector3D translation, Object* object);

	/// <summary>
	/// Full transformation with ability to flip normals
	/// </summary>
	/// <param name="flipNormals"></param>
	/// <param name="rotation"></param>
	/// <param name="translation"></param>
	/// <param name="object"></param>
	TransformedObject(const bool flipNormals, const Vector3D rotation, const Vector3D translation, Object* object);

	virtual ~TransformedObject();

	virtual bool Hit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

	virtual bool SphereIntersectGround(const Vector3D pos, const float radius);
	virtual bool SphereIntersectSphere(const Vector3D pos, const float radius);
private:
	bool m_flipNormals;
	Object* m_object;
	Quaternion m_xAxisRotation;
	Quaternion m_yAxisRotation;
	Quaternion m_zAxisRotation;
	Vector3D m_rotation;
	Vector3D m_scale;
	Vector3D m_translation;

	bool ScaleHit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

	bool RotationHit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

	bool TranslationHit(Ray& ray, const float t_min, const float t_max, HitRec& rec);

	//Object* m_scaledObject;
	//Object* m_rotatedObject;
	//Object* m_rotatedObject;
	//Object* m_translatedObject;
};
