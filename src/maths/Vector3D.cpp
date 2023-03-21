#include "../misc/Random.h"

#include "Vector3D.h"

Vector3D::Vector3D(const Float scalar, const bool includeZAxis) {
	m_x = scalar;
	m_y = scalar;
	m_z = includeZAxis ? scalar : 0;
	m_includeZAxis = includeZAxis;
}

Vector3D::Vector3D(const Float x, const Float y) {
	m_x = x;
	m_y = y;
	m_z = 0;
	m_includeZAxis = false;
}

Vector3D::Vector3D(const Float x, const Float y, const Float z) {
	m_x = x;
	m_y = y;
	m_z = z;
	m_includeZAxis = true;
}

Float Vector3D::DotProduct(const Vector3D& v1, const Vector3D& v2) {
	return (v1.m_x * v2.m_x) + (v1.m_y * v2.m_y) + (v1.m_z * v2.m_z);
}

Float Vector3D::ToroidalDistance(const Vector3D& a, const Vector3D& b, const Vector3D& min, const Vector3D& max) {
	Vector3D delta = Vector3D::Abs(b - a);
	Vector3D minMaxDelta = Vector3D::Abs(max - min);
	Vector3D mid = minMaxDelta / 2;

	if (delta.m_x > mid.m_x) delta.m_x = minMaxDelta.m_x - delta.m_x;
	if (delta.m_y > mid.m_y) delta.m_y = minMaxDelta.m_y - delta.m_y;

	if (delta.m_includeZAxis) {
		if (delta.m_z > mid.m_z) delta.m_z = minMaxDelta.m_z - delta.m_z;
	}
	else {
		delta.m_z = 0;
	}

	return Float::Abs(delta.SqrMagnitude());
}

Vector3D Vector3D::Abs(const Vector3D& v) {
	return v.m_includeZAxis ?
		Vector3D(Float::Abs(v.m_x), Float::Abs(v.m_y), Float::Abs(v.m_z)) :
		Vector3D(Float::Abs(v.m_x), Float::Abs(v.m_y));
}

Vector3D Vector3D::Clamp(const Vector3D& val, const Vector3D& min, const Vector3D& max) {
	return val.m_includeZAxis ?
		Vector3D(Float::Clamp(val.m_x, min.m_x, max.m_x), Float::Clamp(val.m_y, min.m_y, max.m_y), Float::Clamp(val.m_z, min.m_z, max.m_z)) :
		Vector3D(Float::Clamp(val.m_x, min.m_x, max.m_x), Float::Clamp(val.m_y, min.m_y, max.m_y));
}

Vector3D Vector3D::CrossProduct(const Vector3D& v1, const Vector3D& v2) {
	return Vector3D(v1.m_y * v2.m_z - v1.m_z * v2.m_y,
		v1.m_z * v2.m_x - v1.m_x * v2.m_z,
		v1.m_x * v2.m_y - v1.m_y * v2.m_x);
}

Vector3D Vector3D::Lerp(const Vector3D& min, const Vector3D& max, const Float factor, const bool clamp) {
	Float x = Float::Lerp(min.m_x, max.m_x, factor, clamp);
	Float y = Float::Lerp(min.m_y, max.m_y, factor, clamp);
	Float z = Float::Lerp(min.m_z, max.m_z, factor, clamp);

	if (min.m_includeZAxis || max.m_includeZAxis) {
		return Vector3D(x, y, z);
	}
	else {
		return Vector3D(x, y);
	}
}

Vector3D Vector3D::Reflect(const Vector3D& vector, const Vector3D& normal) {
	Vector3D t = Vector3D::DotProduct(normal, vector);
	t *= normal * 2;
	t = vector - t;

	if (t.NearZero()) t = normal;
	t.Normalize();
	return t;
}

Vector3D Vector3D::Refract(const Vector3D& vector, const Vector3D& normal, const Float refractionRatio) {
	Vector3D vectorInv = -vector;

	Float cosTheta = Float::Min(Vector3D::DotProduct(vectorInv, normal), 1);
	Vector3D rOutPerp = (vector + (normal * cosTheta)) * refractionRatio;
	Vector3D rOutPara = normal * -Float::Sqrt(Float::Abs(1 - rOutPerp.SqrMagnitude()));

	Float sinTheta = Float::Sqrt(1 - cosTheta * cosTheta);

	bool cannotRefract = refractionRatio * sinTheta > 1;

	Vector3D refract = rOutPerp + rOutPara;
	if (!cannotRefract) refract.Normalize();

	return cannotRefract ? Vector3D::Reflect(vector, normal) : refract;
}

Vector3D Vector3D::RandomInHemisphere(const Vector3D& normal) {
	Vector3D o = Vector3D::RandomInUnitSphere();
	if (Vector3D::DotProduct(normal, o) < 0) {
		o *= -1;
	}
	return o;
}

Vector3D Vector3D::RandomInUnitDisk() {
	Vector3D o = Vector3D::RandomVector(-1, 1, false);
	//Vector3D o = Vector3D::RandomVector(-1, 1, true);
	//o *= Vector3D(1, 1, 0);

	if (o.SqrMagnitude() > 1) o.Normalize();
	return o;
}

Vector3D Vector3D::RandomInUnitSphere() {
	Vector3D o = RandomVector(-1, 1);

	if (o.SqrMagnitude() > 1) o.Normalize();
	return o;
}

Vector3D Vector3D::RandomUnitVector() {
	Vector3D o = Vector3D::RandomVector(-1, 1);
	o.Normalize();
	return o;
}

Vector3D Vector3D::RandomVector(const Float min, const Float max, bool includeZAxis) {
	return includeZAxis ?
		Vector3D(Random::RandomFloat(min, max), Random::RandomFloat(min, max), Random::RandomFloat(min, max)) :
		Vector3D(Random::RandomFloat(min, max), Random::RandomFloat(min, max));
}
