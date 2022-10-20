#pragma once

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

class BlueNoise {
public:
	BlueNoise() {};
	~BlueNoise() {};

	void Generate(const int& sampleCount, const int& m);

	size_t Size() const { return m_samples.size(); };
	sf::Vector2f& operator[](size_t index);


private:
	std::vector<sf::Vector2f> m_samples;
};