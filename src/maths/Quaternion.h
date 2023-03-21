#ifndef QUATERNION_H
#define QUATERNION_H

#include "../wrapper/Float.h"
#include "Vector3D.h"

class Quaternion {
public:
	Quaternion(const Float w = 0, const Float i = 0, const Float j = 0, const Float k = 0);
	Quaternion(const Quaternion& other);
	Quaternion(const Vector3D& vec);
	~Quaternion() {};

	inline Quaternion& operator*=(const Quaternion& otherQuaternion) {
		Float w = (m_w * otherQuaternion.m_w) - (m_i * otherQuaternion.m_i) - (m_j * otherQuaternion.m_j) - (m_k * otherQuaternion.m_k);
		Float i = (m_w * otherQuaternion.m_i) + (m_i * otherQuaternion.m_w) + (m_j * otherQuaternion.m_k) - (m_k * otherQuaternion.m_j);
		Float j = (m_w * otherQuaternion.m_j) - (m_i * otherQuaternion.m_k) + (m_j * otherQuaternion.m_w) + (m_k * otherQuaternion.m_i);
		Float k = (m_w * otherQuaternion.m_k) + (m_i * otherQuaternion.m_j) - (m_j * otherQuaternion.m_i) + (m_k * otherQuaternion.m_w);

		m_w = w;
		m_i = i;
		m_j = j;
		m_k = k;
		return *this;
	};
	inline Quaternion& operator=(const Quaternion& otherQuaternion) {
		if (this == &otherQuaternion) return *this;
		m_w = otherQuaternion.m_w;
		m_i = otherQuaternion.m_i;
		m_j = otherQuaternion.m_j;
		m_k = otherQuaternion.m_k;
		return *this;
	};
	inline Quaternion& operator=(const Vector3D& otherVector) {
		Vector3D copyVector = otherVector;
		m_w = 0;
		m_i = copyVector.GetX();
		m_j = copyVector.GetY();
		m_k = copyVector.GetZ();
		return *this;
	};

	inline Quaternion operator*(const Quaternion& otherQuaternion) const {
		Quaternion out(m_w, m_i, m_j, m_k);
		out *= otherQuaternion;
		return out;
	};

	// ----- OTHER -----

	inline Vector3D ToVector() { return Vector3D(m_i, m_j, m_k); };
	inline void Conjugate() { m_i *= -1; m_j *= -1; m_k *= -1; };
	inline void Normalize() {
		Float mag = (m_w * m_w) + (m_i * m_i) + (m_j * m_j) + (m_k * m_k);
		mag = Float::Sqrt(mag);
		m_w /= mag;
		m_i /= mag;
		m_j /= mag;
		m_k /= mag;
	};

	// ----- ROTATION -----

	void AxisRotation(const Vector3D& axis, const Float radians);
	static Quaternion ShortestArc(const Vector3D& v1, const Vector3D& v2);

	void XYZRotation(const Vector3D& radians);
	void ZYXRotation(const Vector3D& radians);

	Vector3D RotateVector(const Vector3D& vec, const bool normalize = false) const;

	// ----- STATIC VARIABLES -----

	static const Quaternion Identity;

	// ----- IOSTREAM -----

	friend std::ostream& operator<<(std::ostream& os, const Quaternion& num);

private:
	Float m_w, m_i, m_j, m_k;
};

#endif // !QUATERNION_H
