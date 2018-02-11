#pragma once


#include <SFML/Graphics.hpp>
#include "windows.h"
#include "stdafx.h"
#include <string>
#include <iostream>
#include "robot.h"

using namespace std;

class renderer {
	private:
		sf::RenderWindow window;
		float d;
	
	public:
		renderer();
		void update();
};

renderer::renderer() : 
	window(sf::VideoMode(200, 200), "GA"),
	d(100.f)
{

}

void renderer::update() {
	robot* r = new robot();

	d--;
	if (d <= 0) {
		d = 100.f;
	}
	cout << d << "\n";
	sf::CircleShape shape(d);
	shape.setFillColor(sf::Color::Green);
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();
	}

	window.clear();
	window.draw(shape);
	window.display();
}