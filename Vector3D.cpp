#include <cmath>

#include "Vector3D.h"
#include "LinearFeedbackShift.h"

Vector3D::Vector3D() {
	m_x = 0.0f;
	m_y = 0.0f;
	m_z = 0.0f;
}

Vector3D::Vector3D(const float x, const float y, const float z) {
	m_x = x;
	m_y = y;
	m_z = z;
}

Vector3D::Vector3D(const Vector3D& copyVector) {
	m_x = copyVector.m_x;
	m_y = copyVector.m_y;
	m_z = copyVector.m_z;
}

/// <summary>
/// Assignment operator
/// </summary>
/// <param name="copyVector3d"></param>
/// <returns></returns>
Vector3D& Vector3D::operator=(const Vector3D& copyVector) {
	if (&copyVector == this) return *this;

	m_x = copyVector.m_x;
	m_y = copyVector.m_y;
	m_z = copyVector.m_z;

	return *this;
}

/// <summary>
/// Dividing two vectors
/// </summary>
/// <param name="copyVector3d"></param>
/// <returns></returns>
const Vector3D Vector3D::operator/(const Vector3D& copyVector) const {
	Vector3D temp(*this);
	temp /= copyVector;
	return temp;
}

/// <summary>
/// Dividing two vectors
/// </summary>
/// <param name="copyVector3d"></param>
/// <returns></returns>
Vector3D& Vector3D::operator/=(const Vector3D& copyVector) {
	m_x /= copyVector.m_x;
	m_y /= copyVector.m_y;
	m_z /= copyVector.m_z;

	return *this;
}

/// <summary>
/// Divide vector by a scalar
/// </summary>
/// <param name="scalar"></param>
/// <returns></returns>
const Vector3D Vector3D::operator/(const float scalar) const {
	Vector3D temp(*this);
	temp /= scalar;
	return temp;
}

/// <summary>
/// Divide vector by a scalar
/// </summary>
/// <param name="scalar"></param>
/// <returns></returns>
Vector3D& Vector3D::operator/=(const float scalar) {
	m_x /= scalar;
	m_y /= scalar;
	m_z /= scalar;

	return *this;
}

/// <summary>
/// Multiplying two vectors
/// </summary>
/// <param name="copyVector3d"></param>
/// <returns></returns>
const Vector3D Vector3D::operator*(const Vector3D& copyVector) const {
	Vector3D temp(*this);
	temp *= copyVector;
	return temp;
}

/// <summary>
/// Multiplying two vectors
/// </summary>
/// <param name="copyVector3d"></param>
/// <returns></returns>
Vector3D& Vector3D::operator*=(const Vector3D& copyVector) {
	m_x *= copyVector.m_x;
	m_y *= copyVector.m_y;
	m_z *= copyVector.m_z;

	return *this;
}

/// <summary>
/// Multiply vector by a scalar
/// </summary>
/// <param name="scalar"></param>
/// <returns></returns>
const Vector3D Vector3D::operator*(const float scalar) const {
	Vector3D temp(*this);
	temp *= scalar;
	return temp;
}

/// <summary>
/// Multiply vector by a scalar
/// </summary>
/// <param name="scalar"></param>
/// <returns></returns>
Vector3D& Vector3D::operator*=(const float scalar) {
	m_x *= scalar;
	m_y *= scalar;
	m_z *= scalar;

	return *this;
}

const Vector3D Vector3D::operator+(const Vector3D& copyVector) const {
	Vector3D temp(*this);
	temp += copyVector;
	return temp;
}

Vector3D& Vector3D::operator+=(const Vector3D& copyVector) {
	m_x += copyVector.m_x;
	m_y += copyVector.m_y;
	m_z += copyVector.m_z;

	return *this;
}

const Vector3D Vector3D::operator-(const Vector3D& copyVector) const {
	Vector3D temp(*this);
	temp -= copyVector;
	return temp;
}

Vector3D& Vector3D::operator-=(const Vector3D& copyVector) {
	m_x -= copyVector.m_x;
	m_y -= copyVector.m_y;
	m_z -= copyVector.m_z;

	return *this;
}
/// <summary>
/// Magnitude of vector
/// </summary>
/// <returns></returns>
const float Vector3D::Magnitude() {
	return sqrt(SqrMagnitude());
}

/// <summary>
/// Get length of vector squared
/// </summary>
/// <returns></returns>
const
float Vector3D::SqrMagnitude() {
	return m_x * m_x + m_y * m_y + m_z * m_z;
}

const float Vector3D::DotProduct(const Vector3D& copyVector) {
	return m_x * copyVector.m_x + m_y * copyVector.m_y + m_z * copyVector.m_z;
}

const Vector3D Vector3D::CrossProduct(const Vector3D& copyVector) {
	Vector3D temp = Vector3D(m_y * copyVector.m_z - m_z * copyVector.m_y, m_z * copyVector.m_x - m_x * copyVector.m_z, m_x * copyVector.m_y - m_y * copyVector.m_x);
	return temp;
}

const Vector3D Vector3D::UnitVector() {
	float mag = Magnitude();
	Vector3D temp = Vector3D(m_x / mag, m_y / mag, m_z / mag);
	return temp;
}

//void Vector3D::CrossProduct(const Vector3D& copyVector) {
//	m_x = m_y * copyVector.m_z - m_z * copyVector.m_y;
//	m_y = m_z * copyVector.m_x - m_x * copyVector.m_z;
//	m_z = m_x * copyVector.m_y - m_y * copyVector.m_x;
//}
//
//void Vector3D::UnitVector() {
//	float mag = Magnitude();
//	m_x /= mag;
//	m_y /= mag;
//	m_z /= mag;
//}

Vector3D Vector3D::Random() {
	return Vector3D(LinearFeedbackShift::RandFloat(16), LinearFeedbackShift::RandFloat(16), LinearFeedbackShift::RandFloat(32));
}

Vector3D Vector3D::Random(const float min, const float max) {
	return Vector3D(LinearFeedbackShift::RandFloatRange(min, max, 16), LinearFeedbackShift::RandFloatRange(min, max, 16), LinearFeedbackShift::RandFloatRange(min, max, 32));
}

Vector3D Vector3D::RandomInHemisphere(const Vector3D& normal) {
	Vector3D inUnitSphere = RandomInUnitSphere();
	//inUnitSphere.UnitVector();
	if (inUnitSphere.DotProduct(normal) > 0.0f) {
		return inUnitSphere;
	}
	else {
		inUnitSphere *= 1.0f;
		return inUnitSphere;
	}

	return inUnitSphere;
}

Vector3D Vector3D::RandomInUnitSphere() {
	while (true) {
		Vector3D p = Vector3D::Random(-1.0f, 1.0f);

		if (p.SqrMagnitude() >= 1) continue;

		return p;
	}

	return Vector3D();
}

Vector3D Vector3D::RandomUnitVector() {
	Vector3D temp = RandomInUnitSphere().UnitVector();
	//temp.UnitVector();
	return temp;
}

Vector3D::~Vector3D() {
}