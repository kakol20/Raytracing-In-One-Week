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
	static Quaternion RotationQuat(const Quaternion& rotation, const Vector3D& point);
	static Vector3D RotationVec(const Quaternion& rotation, const Vector3D& v);

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

	static Quaternion VectorToPure(const Vector3D& v);

	/// <summary>
	/// Convert Axis Rotation to Quaternion Rotation
	/// </summary>
	/// <param name="axis"></param>
	/// <param name="radians">in degrees</param>
	/// <returns></returns>
	static Quaternion AxisToRotation(const Vector3D& axis, const float radians);

private:
	float m_w;
	float m_i;
	float m_j;
	float m_k;
};

