#include <iostream>
#include <string>

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "utility/Color.h"

#ifdef _DEBUG
int main() {
#else
int WinMain() {
#endif // _DEBUG

	const unsigned int width = 1280;
	const unsigned int height = 720;
	const std::string windowName = "Raytracing";
	const sf::Uint32 style = sf::Style::Titlebar | sf::Style::Close;

	sf::RenderWindow window(sf::VideoMode(width, height), windowName, style);

	Color col1(1.f, 128.f, 64.f);
	Color col2(64.f, 255.f, 128.f);
	Color col3 = col1 / col2;
	Color::RawValue col3Raw = col3.GetRawValue();
	Color col4(col3Raw);

	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(shape);
		window.display();
	}

	return  0;
}