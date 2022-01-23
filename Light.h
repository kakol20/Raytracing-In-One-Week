#pragma once
#include "Vector3D.h"

class Light {
public:
	Light();
	Light(const Vector3D& pos, const Vector3D& color);
	~Light();

	Light& operator=(const Light& copyLight);

	const Vector3D GetColor();
	const Vector3D GetPosition();

private:
	Vector3D m_position;
	Vector3D m_color;
};
