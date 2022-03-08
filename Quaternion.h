#pragma once
#include "Vector3D.h"

class Quaternion {
public:
	Quaternion() : Quaternion(0.f, 1.f, 0.f, 0.f) {};
	Quaternion(const float w, const float i, const float j, const float k);
	~Quaternion();

	// ----- OPERATOR OVERLOADS -----

	/// <summary>
	/// Simple Assignment
	/// </summary>
	/// <param name="copyQuaternion"></param>
	/// <returns></returns>
	Quaternion& operator=(const Quaternion& copyQuaternion);

	// Hamilton product
	static Quaternion HamProduct(const Quaternion& q1, const Quaternion& q2);

	// ----- GETTERS & SETTERS -----

	Vector3D GetIJK();

private:
	float m_w;
	float m_i;
	float m_j;
	float m_k;
};

