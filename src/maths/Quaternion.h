#pragma once

#include <cmath>

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "Vector3.h"

class Quaternion {
public:
	Quaternion(const float& w = 0.f, const float& i = 0.f, const float& j = 0.f, const float& k = 0.f);
	Quaternion(const sf::Vector3f& vec);

	inline Quaternion& operator*=(const Quaternion& otherQuaternion) {
		float w = (m_w * otherQuaternion.m_w) - (m_i * otherQuaternion.m_i) - (m_j * otherQuaternion.m_j) - (m_k * otherQuaternion.m_k);
		float i = (m_w * otherQuaternion.m_i) + (m_i * otherQuaternion.m_w) + (m_j * otherQuaternion.m_k) - (m_k * otherQuaternion.m_j);
		float j = (m_w * otherQuaternion.m_j) - (m_i * otherQuaternion.m_k) + (m_j * otherQuaternion.m_w) + (m_k * otherQuaternion.m_i);
		float k = (m_w * otherQuaternion.m_k) + (m_i * otherQuaternion.m_j) - (m_j * otherQuaternion.m_i) + (m_k * otherQuaternion.m_w);

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
	inline Quaternion& operator=(const sf::Vector3f& otherVector) {
		sf::Vector3f copyVector = otherVector;
		m_w = 0;
		m_i = copyVector.x;
		m_j = copyVector.y;
		m_k = copyVector.z;
		return *this;
	};

	inline Quaternion operator*(const Quaternion& otherQuaternion) const {
		Quaternion out(m_w, m_i, m_j, m_k);
		out *= otherQuaternion;
		return out;
	};

	// ----- OTHER -----

	inline sf::Vector3f ToVector() { return sf::Vector3f(m_i, m_j, m_k); };
	inline void Conjugate() { m_i *= -1; m_j *= -1; m_k *= -1; };
	inline void Normalize() {
		float mag = (m_w * m_w) + (m_i * m_i) + (m_j * m_j) + (m_k * m_k);
		mag = std::sqrtf(mag);
		m_w /= mag;
		m_i /= mag;
		m_j /= mag;
		m_k /= mag;
	};

	// ----- ROTATION -----

	void AxisRotation(const sf::Vector3f& axis, const float& radians);
	void ShortestArc(const sf::Vector3f& v1, const sf::Vector3f& v2);

	void XYZRotation(const sf::Vector3f& radians);
	void ZYXRotation(const sf::Vector3f& radians);

	sf::Vector3f RotateVector(const sf::Vector3f& vec, const bool& normalize = false);

	// ----- STATIC VARIABLES -----

	static const Quaternion Identity;

private:
	float m_w, m_i, m_j, m_k;
};