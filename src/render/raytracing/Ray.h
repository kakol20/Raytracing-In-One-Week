#pragma once

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "../../maths/Vector3.h"

class Ray {
public:
	Ray() : Ray(rt::Vector3::Zero, rt::Vector3::Forward) {};
	Ray(const sf::Vector3f& orig, const sf::Vector3f& dir);
	~Ray() {};

	Ray& operator=(const Ray& copyRay);

	sf::Vector3f At(const float& t) const;

	sf::Vector3f GetDir() const { return m_dir; };
	void SetDir(const sf::Vector3f& dir) { m_dir = dir; };

	sf::Vector3f GetOrig() const { return m_orig; };
	void SetOrig(const sf::Vector3f orig) { m_orig = orig; };

private:
	sf::Vector3f m_dir;
	sf::Vector3f m_orig;
};