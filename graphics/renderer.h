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
		sf::RenderWindow _window;
		int _height;
		int _width;
		float modY(float y);
	
	public:
		renderer(int height, int width);
		void update(robot* r);
		void drawRobot(robot * r);
		void drawJoint(point * p, int weight);
		void drawLine(point * a, point * b, int strength);
};

renderer::renderer(int height, int width) : 
	_window(sf::VideoMode(height, width), "GA"),
	_height(height),
	_width(width)
{

}

void renderer::update(robot* r) {
	

	sf::Event event;
	while (_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			_window.close();
	}

	r->tick();

	_window.clear();
	drawRobot(r);
	_window.display();
}

float renderer::modY(float y) {
	return _height - (y + 20);
}

void renderer::drawRobot(robot * r) {

	for (int i = 0; i < r->muscles.size(); i++) {
		drawLine(r->muscles[i]->a->position, r->muscles[i]->b->position, r->muscles[i]->strength);
	}
	for (int i = 0; i < r->joints.size(); i++) {
		drawJoint(r->joints[i]->position, r->joints[i]->weight);
	}

}

void renderer::drawJoint(point * p, int weight) {
	float radius = 4.f;

	sf::CircleShape shape(radius);
	shape.setFillColor(sf::Color::Green);
	shape.setPosition(p->x - radius, modY(p->y) - radius);
	_window.draw(shape);
}

void renderer::drawLine(point * a, point * b, int strength) {

	sf::Color c = sf::Color(255 - strength, 50, 50, 255);

	sf::Vertex line[] =
	{
		sf::Vertex(sf::Vector2f(a->x, modY(a->y))),
		sf::Vertex(sf::Vector2f(b->x, modY(b->y)))
	};

	line[0].color = c;
	line[1].color = c;

	_window.draw(line, 2, sf::Lines);
}