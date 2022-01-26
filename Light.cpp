#include "Light.h"

Light::Light() {
	m_position = Vector3D();
	m_color = Vector3D();
}

Light::Light(const Vector3D& pos, const Vector3D& color, const float intensity) {
	m_position = pos;
	m_color = color;
	m_intensity = intensity;
}

Light::~Light() {
}

Light& Light::operator=(const Light& copyLight) {
	// TODO: insert return statement here
	if (this == &copyLight) return *this;

	m_position = copyLight.m_position;
	m_color = copyLight.m_color;
	m_intensity = copyLight.m_intensity;

	return *this;
}

const Vector3D Light::GetColor() {
	return m_color;
}

const Vector3D Light::GetPosition() {
	return m_position;
}

const float Light::GetIntensity() {
	return m_intensity;
}
