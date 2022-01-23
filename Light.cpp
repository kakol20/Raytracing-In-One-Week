#include "Light.h"

Light::Light() {
	m_position = Vector3D();
	m_color = Vector3D();
}

Light::Light(const Vector3D& pos, const Vector3D& color) {
	m_position = pos;
	m_color = color;
}

Light::~Light() {
}

Light& Light::operator=(const Light& copyLight) {
	// TODO: insert return statement here
	if (this == &copyLight) return *this;

	m_position = copyLight.m_position;
	m_color = copyLight.m_color;

	return *this;
}

const Vector3D Light::GetColor() {
	return m_color;
}

const Vector3D Light::GetPosition() {
	return m_position;
}
