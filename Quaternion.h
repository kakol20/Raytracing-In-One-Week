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

	// ----- OTHER -----

	// Hamilton product
	static Quaternion HamProduct(const Quaternion& q1, const Quaternion& q2);

	static Quaternion RotationQuat(const Quaternion& rotation, const Quaternion& point);

	// https://ntrs.nasa.gov/citations/19770024290

	/// <summary>
	/// YXZ Rotation
	/// </summary>
	/// <param name="y">in radians</param>
	/// <param name="x">in radians</param>
	/// <param name="z">in radians</param>
	/// <returns></returns>
	static Quaternion ToQuaternionYXZ(const float y, const float x, const float z);

	/// <summary>
	/// ZXY Rotation
	/// </summary>
	/// <param name="z">in radians</param>
	/// <param name="x">in radians</param>
	/// <param name="y">in radians</param>
	/// <returns></returns>
	static Quaternion ToQuaternionZXY(const float z, const float x, const float y);

	void Conjugate();
	void Normalize();

	// ----- GETTERS & SETTERS -----

	Vector3D GetIJK();

private:
	float m_w;
	float m_i;
	float m_j;
	float m_k;
};

