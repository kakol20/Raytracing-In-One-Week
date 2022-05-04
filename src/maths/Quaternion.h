#ifndef QUATERNION_H
#define QUATERNION_H

#include "../wrapper/Float.h"
#include "Vector3D.h"

class Quaternion {
public:
	Quaternion(const Float& w = 0, const Float& i = 0, const Float& j = 0, const Float& k = 0);
	Quaternion(const Vector3D& vec);
	~Quaternion() {};

	Quaternion& operator*=(const Quaternion& otherQuaternion);
	Quaternion& operator=(const Quaternion& otherQuaternion);
	Quaternion& operator=(const Vector3D& otherVector);

	Quaternion operator*(const Quaternion& otherQuaternion) const;

	// ----- OTHER -----

	//Quaternion Conjugate();

	Vector3D ToVector();
	void Conjugate();
	void Normalize();

	// ----- ROTATION -----

	void AxisRotation(const Vector3D& axis, const Float& radians);
	void ShortestArc(const Vector3D& v1, const Vector3D& v2);

	void XYZRotation(const Vector3D& radians);
	void ZYXRotation(const Vector3D& radians);

	Vector3D RotateVector(const Vector3D& vec, const bool& normalize = false);

	// ----- STATIC VARIABLES -----

	static const Quaternion Identity;

	// ----- IOSTREAM -----

	friend std::ostream& operator<<(std::ostream& os, const Quaternion& num);

private:
	Float m_w, m_i, m_j, m_k;
};

#endif // !QUATERNION_H
