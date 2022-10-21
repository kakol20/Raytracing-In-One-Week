#pragma once

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "../../maths/Vector3.h"
#include "Ray.h"

class Camera {
public:
	Camera() : Camera(1, 0, 10, 90, rt::Vector3::Zero, rt::Vector3::Forward, rt::Vector3::Up) {};
	Camera(const Camera& copyCamera);
	Camera(const float& aspectRatio, const float& blurStrength, const float& focusDist, const float& vFOV, const sf::Vector3f& lookFrom, const sf::Vector3f& lookAt, const sf::Vector3f& up);
	~Camera() {};

	Camera& operator=(const Camera& copyCamera);

	Ray GetRay(const float& s, const float& t) const;

private:
	float m_lensRadius;
	sf::Vector3f m_horizontal;
	sf::Vector3f m_lowerLeftCorner;
	sf::Vector3f m_origin;
	sf::Vector3f m_u;
	sf::Vector3f m_v;
	sf::Vector3f m_vertical;
	sf::Vector3f m_w;
};